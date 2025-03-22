#include "FatDirectory.h"
#include "FatFile.h"

#define ENTRYPOS(directory, index, pos)			\
    (fatunitgetdata((directory))[(index) * 32 + (pos)])

namespace flippy {

FatDirectory::FatDirectory(std::shared_ptr<FatFat> fat)
    : mFat(std::move(fat)), mPath(u8"/")
{
    mTarget = fatgetrootbegin(mFat->f);
    mFirst = FAT_FIRST;
    mLast = fatlastcluster(mFat->f);
    mFat->f->insensitive = 1;
    if (mTarget == FAT_ERR) {
        return;
    }

    auto dir = fatclusterread(mFat->f, mTarget);
    if (dir == nullptr) {
        return;
    }

    mDirectory = dir;
    mIndex = 0;
}

FatDirectory::FatDirectory(std::shared_ptr<FatFat> fat, const std::filesystem::path& path, int32_t target)
    : mFat(std::move(fat)), mPath(path.u8string() + u8'/')
{
    mTarget = (target == FAT_ERR) ? fatgetrootbegin(mFat->f) : target;
    mFirst = FAT_FIRST;
    mLast = fatlastcluster(mFat->f);
    mFat->f->insensitive = 1;
    if (mTarget == FAT_ERR) {
        return;
    }

    if (fatinvalidpath(path.c_str()) < 0) {
        return;
    }

    auto npath = fatstoragepath(path.c_str());

    unit* directory = nullptr;
    int index = 0;

    int res = fatlookuppath(mFat->f, mTarget, npath, &directory, &index);
    mTarget = res ? fatlookuppathfirstcluster(mFat->f, mTarget, npath) : fatreferencegettarget(mFat->f, directory, index, 0);

    free(npath);

    if (res && mTarget == FAT_ERR) {
        return;
    }

    mDirectory = directory;
    mIndex = index;
}

FatDirectory::~FatDirectory()
{
}

std::vector<Entry> FatDirectory::buildEntries(unit* startDir, int startIndex) const
{
    std::vector<Entry> entries;

    int32_t index, res;
    unit* dir = startDir;
    for (index = startIndex, res = 0; res == 0; res = fatnextentry(mFat->f, &dir, &index)) {
        if (!fatentryexists(dir, index)) {
            continue;
        }

        char shortname[13];
        fatshortnametostring(shortname, &ENTRYPOS(dir, index, 0));
        const auto attrs = fatentrygetattributes(dir, index);

        if (attrs & FAT_ATTR_DIR) {
            // directory
            entries.push_back(Entry(std::shared_ptr<FatDirectory>(new FatDirectory(mFat, std::filesystem::path(shortname), mTarget))));
        } else if (!(attrs & FAT_ATTR_VOLUME)) {
            // file
            entries.push_back(Entry(std::shared_ptr<FatFile>(new FatFile(mFat, dir, index))));
        }
    }

    return std::move(entries);
}

Result<std::vector<Entry>> FatDirectory::ls() const
{
    if (mTarget == FAT_ERR) {
        return std::unexpected(Error("Invalid directory"));
    }
    return buildEntries(mDirectory, mIndex);
}

Result<void> FatDirectory::chdir(std::filesystem::path name)
{
}

Result<void> FatDirectory::mkdir(std::filesystem::path name)
{
}

Result<void> FatDirectory::rm(std::filesystem::path name)
{
}

Result<void> FatDirectory::copy(std::filesystem::path name)
{
}

Result<void> FatDirectory::rename(std::filesystem::path oldName, std::filesystem::path newName)
{
}

Result<std::shared_ptr<File>> FatDirectory::open(std::filesystem::path name, OpenMode mode)
{
}

Result<std::filesystem::path> FatDirectory::path() const
{
    return mPath;
}

} // namespace flippy
