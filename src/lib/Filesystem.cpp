#include "Filesystem.h"
#include "FatDirectory.h"
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
extern "C" {
#include <llfat.h>
}

namespace flippy {

struct FatFormat
{
    std::size_t headerByteSize;
    std::size_t dataByteSize;

    std::size_t sectorByteSize;
    std::size_t sectorsPerCluster;
    std::size_t sectorsPerTrack;
    std::size_t surfaceCount;
    std::size_t cylinderCount;
    std::size_t maxRootEntries;
};

std::unordered_map<Format, FatFormat> formatInfo = {
    { Format::FDI, { 4096, 1261568, 1024, 1, 8, 2, 77, 192 } },
    { Format::HDM, {    0, 1261568, 1024, 1, 8, 2, 77, 192 } },
};

static inline Format formatFromPath(std::filesystem::path path)
{
    if (path.extension() == ".fdi") {
        return Format::FDI;
    } else if (path.extension() == ".hdm") {
        return Format::HDM;
    }
    return Format::Auto;
}

Result<std::shared_ptr<Directory>> Filesystem::root(std::filesystem::path path, Format format)
{
    // open existing file
    if (format == Format::Auto) {
        format = formatFromPath(path);
    }
    auto info = formatInfo.find(format);
    if (info == formatInfo.end()) {
        return std::unexpected(Error("root: Unsupported format"));
    }

    const std::size_t offset = info->second.headerByteSize;
    char* filename = strdup(path.c_str());
    fat* f = fatopen(filename, offset);
    if (f == nullptr) {
        free(filename);
        return std::unexpected(Error("root: Could not open FAT filesystem"));
    }
    int res = fatcheck(f);
    if (res) {
        free(filename);
        return std::unexpected(Error("root: Invalid FAT filesystem"));
    }

    return std::make_shared<FatDirectory>(std::make_shared<FatFat>(f));
}

// lifted from fattool.c
static inline Error toosmall(fat* f)
{
    uint32_t nsectors;
    int fatsize;

    std::string message;
    auto to = std::back_inserter(message);

    auto finalize = [&message, &to, f]() {
        std::format_to(to, " > {}", fatgetnumsectors(f));
        return Error(std::move(message));
    };

    std::format_to(to, "create: Too small: ");

    nsectors = 0;
    std::format_to(to, "{}", fatgetreservedsectors(f));
    nsectors += fatgetreservedsectors(f);
    if (nsectors > fatgetnumsectors(f))
        return finalize();

    fatsize = fatnumdataclusters(f) < 0 ? 1 : fatgetfatsize(f);
    std::format_to(to, " + {}*{}", fatsize, fatgetnumfats(f));
    nsectors += fatsize * fatgetnumfats(f);
    if (nsectors > fatgetnumsectors(f))
        return finalize();

    std::format_to(to, " + {}*32/{}", fatgetrootentries(f), fatgetbytespersector(f));
    nsectors += fatgetrootentries(f) * 32 / fatgetbytespersector(f);
    if (nsectors > fatgetnumsectors(f))
        return finalize();

    std::format_to(to, " + {}", fatgetsectorspercluster(f) * (fatbits(f) == 32 ? 2 : 1));
    nsectors += fatgetsectorspercluster(f) * (fatbits(f) == 32 ? 2 : 1);
    if (nsectors > fatgetnumsectors(f))
        return finalize();

    return Error("create: FAT filesystem is too small");
}

static inline void fatsetsize(fat *f) {
    int bits, extra, best;

    extra = 0;

    for (bits = 12; ; bits = fatbits(f)) {
        f->bits = bits;
        fatsetreservedsectors(f, bits == 32 ? 32 : 1);
        best = fatbestfatsize(f);
        fatsetfatsize(f, best + extra);
        f->bits = fatnumdataclusters(f) < 0 ? 32 : 0;
        if (bits == fatbits(f))
            return;
        if (bits > fatbits(f))
            extra++;
    }
}

static inline void fatzero(fat *f, int table) {
    int nfat;
    unit *root;
    int index;

    fatsetdirtybits(f, 0);

    if (table)
        for (nfat = 0; nfat < fatgetnumfats(f); nfat++)
            fatinittable(f, nfat);
    else if (fatbits(f) == 32)
        fatsetnextcluster(f, fatgetrootbegin(f), FAT_EOF);

    root = fatclusterread(f, fatgetrootbegin(f));
    for (index = 0; index < root->size / 32; index++)
        fatentryzero(root, index);
}

Result<std::shared_ptr<Directory>> Filesystem::create(std::filesystem::path path, Format format)
{
    int rfd = open(path.c_str(), O_RDONLY);
    if (rfd != -1 || errno != ENOENT) {
        if (rfd != -1) {
            close(rfd);
        }
        return std::unexpected(Error("create: File already exists, {} {}", rfd, errno));
    }

    // create new file
    if (format == Format::Auto) {
        format = formatFromPath(path);
    }
    auto infoIt = formatInfo.find(format);
    if (infoIt == formatInfo.end()) {
        return std::unexpected(Error("create: Unsupported format"));
    }

    const auto& info = infoIt->second;
    const std::size_t offset = info.headerByteSize;

    char* filename = strdup(path.c_str());

    const auto sectorCount = info.cylinderCount * info.sectorsPerTrack * info.surfaceCount;

    auto f = fatcreate();
    f->devicename = filename;
    f->offset = offset;
    f->boot = fatunitcreate(info.sectorByteSize);
    f->boot->n = 0;
    f->boot->origin = offset;
    f->boot->dirty = 1;
    fatunitinsert(&f->sectors, f->boot, 1);

    memset(fatunitgetdata(f->boot), 0, f->boot->size);
    fatsetnumfats(f, 2);
    fatsetbytespersector(f, info.sectorByteSize);
    fatsetnumsectors(f, sectorCount);

    if (fatsetsectorspercluster(f, info.sectorsPerCluster)) {
        free(filename);
        fatquit(f);
        return std::unexpected(Error("create: Invalid sectors per cluster: {}", info.sectorsPerCluster));
    }

    fatsetsize(f);
    if (fatnumdataclusters(f) < 0) {
        free(filename);
        fatquit(f);
        return std::unexpected(Error("create: Too many clusters"));
    }
    else if (! fatconsistentsize(f)) {
        free(filename);
        fatquit(f);
        return std::unexpected(toosmall(f));
    }

    if (fatsetrootentries(f, info.maxRootEntries)) {
        free(filename);
        fatquit(f);
        return std::unexpected(Error("create: Invalid number of entries in root: {}", info.maxRootEntries));
    }

    fatsetbootsignature(f);
    fatsetrootbegin(f, fatbits(f) == 32 ? FAT_FIRST : FAT_ROOT);

    if (fatbits(f) != 32)
        f->info = NULL;
    else {
        fataddextendedbootsignature(f);
        fatsetfilesystemtype(f, "FAT32   ");
        srandom(time(NULL));
        fatsetserialnumber(f, random() % 0xFFFFFFFF);

        f->info = fatunitcreate(info.sectorByteSize);
        f->info->n = 1;
        f->info->origin = offset;
        f->info->dirty = 1;
        fatunitinsert(&f->sectors, f->info, 1);
        fatsetinfopos(f, f->info->n);
        fatsetinfosignatures(f);
        fatsetfreeclusters(f, fatlastcluster(f) - 2 - 1);

        fatsetbackupsector(f, 6);
    }

    fatsummary(f);
    if (fatnumdataclusters(f) > 0x0FFFFFFF) {
        free(filename);
        fatquit(f);
        return std::unexpected(Error("create: Too many clusters: not portable"));
    }

    f->fd = open(path.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (f->fd == -1) {
        std::string err;
        err.resize(1024);
        strerror_r(errno, &err[0], err.size());
        err.resize(strlen(&err[0]));
        free(filename);
        fatquit(f);
        return std::unexpected(Error("create: Could not open file: {}", err));
    }

    if (ftruncate(f->fd, f->offset + 1LLU * fatgetnumsectors(f) * fatgetbytespersector(f)) == -1) {
        std::string err;
        err.resize(1024);
        strerror_r(errno, &err[0], err.size());
        err.resize(strlen(&err[0]));
        free(filename);
        fatquit(f);
        return std::unexpected(Error("create: Could not set file size: {}", err));
    }

    f->boot->fd = f->fd;
    if (f->info != NULL)
        f->info->fd = f->fd;

    fatsetmedia(f, 0xF8);
    if (fatbits(f) == 32)
        fatcopyboottobackup(f);
    fatzero(f, 1);

    if (format == Format::FDI) {
        // write 4k header
        // ### should ensure this is always little endian
        std::vector<uint32_t> header;
        header.resize(8);
        header[0] = 0; // reserved
        header[1] = 144; // 144 = 0x90 = 1.2MB 2HD
        header[2] = info.headerByteSize; // 4096
        header[3] = info.dataByteSize; // 1261568
        header[4] = info.sectorByteSize; // 1024
        header[5] = info.sectorsPerTrack; // 8
        header[6] = info.surfaceCount; // 2
        header[7] = info.cylinderCount; // 77

        // get current file position
        const off_t currentPos = lseek(f->fd, 0, SEEK_CUR);
        lseek(f->fd, 0, SEEK_SET);
        // write header
        write(f->fd, header.data(), header.size() * sizeof(uint32_t));
        // seek back
        lseek(f->fd, currentPos, SEEK_SET);
    }
    fatflush(f);

    return std::make_shared<FatDirectory>(std::make_shared<FatFat>(f));
}

} // namespace flippy
