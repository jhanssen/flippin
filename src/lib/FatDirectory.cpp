#include "FatDirectory.h"
#include "FatFile.h"

#define ENTRYPOS(directory, index, pos)			\
    (fatunitgetdata((directory))[(index) * 32 + (pos)])

namespace flippy {

FatDirectory::FatDirectory(std::shared_ptr<FatFat> fat)
    : mFat(std::move(fat)), mShort(u8"/")
{
    mTarget = mRoot = fatgetrootbegin(mFat->f);
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

FatDirectory::FatDirectory(std::shared_ptr<FatFat> fat, std::filesystem::path shortp, std::filesystem::path longp,
                           unit* parentDir, int parentIndex)
    : mFat(std::move(fat)), mShort(std::move(shortp)), mLong(std::move(longp)),
      mParentDirectory(parentDir), mParentIndex(parentIndex)
{
    if (!mLong.empty()) {
        mLong += u8'/';
    }
    if (!mShort.empty()) {
        mShort += u8'/';
    }

    mTarget = (mParentDirectory == nullptr) ? fatgetrootbegin(mFat->f) : mParentDirectory->n;
    mRoot = fatgetrootbegin(mFat->f);
    mFirst = FAT_FIRST;
    mLast = fatlastcluster(mFat->f);
    mFat->f->insensitive = 1;
    if (mTarget == FAT_ERR) {
        return;
    }

    if (fatinvalidpath(mShort.c_str()) < 0) {
        return;
    }

    auto npath = fatstoragepath(mShort.c_str());

    // unit* directory = nullptr;
    // int index = 0;

    //int res = fatlookuppath(mFat->f, mTarget, npath, &directory, &index);
    mTarget = fatlookuppathfirstcluster(mFat->f, mTarget, npath); // : fatreferencegettarget(mFat->f, directory, index, 0);

    free(npath);

    if (mTarget == FAT_ERR) {
        return;
    }

    mDirectory = fatclusterread(mFat->f, mTarget);
    mIndex = 0;
}

FatDirectory::~FatDirectory()
{
}

std::vector<Entry> FatDirectory::buildEntries(unit* startDir, int startIndex) const
{
    std::vector<Entry> entries;

    int32_t index;
    unit* dir = startDir;
    unit* longdir;
    int longindex;
    char* name;
    int res;

    for (index = startIndex; (res = fatlongnext(mFat->f, &dir, &index, &longdir, &longindex, &name)) != FAT_END; fatnextentry(mFat->f, &dir, &index)) {
        if (!fatentryexists(dir, index)) {
            continue;
        }

        std::filesystem::path shortname, longname;
        if (res & FAT_LONG_ALL) {
            longname = name;
        } else if (res & FAT_SHORT) {
            longname = name;
        }
        free(name);

        char shortnamebuf[13];
        fatshortnametostring(shortnamebuf, &ENTRYPOS(dir, index, 0));
        shortname = shortnamebuf;

        const auto attrs = fatentrygetattributes(dir, index);

        if (attrs & FAT_ATTR_DIR) {
            // directory
            if (shortname == "." || shortname == "..") {
                continue;
            }
            entries.push_back(Entry(std::shared_ptr<FatDirectory>(new FatDirectory(mFat, std::move(shortname), std::move(longname), dir, index))));
        } else if (!(attrs & FAT_ATTR_VOLUME)) {
            // file
            entries.push_back(Entry(std::shared_ptr<FatFile>(new FatFile(mFat, std::move(shortname), std::move(longname), dir, index))));
        }
    }

    return std::move(entries);
}

Result<std::vector<Entry>> FatDirectory::dir() const
{
    if (mTarget == FAT_ERR) {
        return std::unexpected(Error("dir: Invalid directory"));
    }
    return buildEntries(mDirectory, mIndex);
}

Result<void> FatDirectory::mkdirShort(const std::filesystem::path& currentPath, const std::filesystem::path& name, bool failIfExists)
{
    unit* dir;
    int index;

    char* npath;
    if (!currentPath.empty()) {
        npath = fatstoragepath(currentPath.c_str());
        if (fatlookuppath(mFat->f, mTarget, npath, &dir, &index) != 0) {
            return std::unexpected(Error("mkdir: pre path does not exist"));
        }
        const auto nd = fatentrygetfirstcluster(dir, index, fatbits(mFat->f));
        dir = fatclusterread(mFat->f, nd);
        free(npath);
    } else {
        dir = mDirectory;
        index = mIndex;
    }

    const auto target = dir->n;
    npath = fatstoragepath(name.c_str());
    // does the path already exist
    if (fatlookupfile(mFat->f, target, npath, &dir, &index) == 0) {
        free(npath);
        if (failIfExists) {
            return std::unexpected(Error("mkdir: path already exists"));
        }
        return {};
    }

    fatcreatefile(mFat->f, target, npath, &dir, &index);

    auto ret = mkdirFinalize(dir, index, target);

    free(npath);

    return std::move(ret);
}

Result<void> FatDirectory::mkdirLong(const std::filesystem::path& currentPath, const std::filesystem::path& name, bool failIfExists)
{
    unit* dir;
    int index;

    char* npath;
    if (!currentPath.empty()) {
        npath = fatstoragepathlong(currentPath.c_str());
        if (fatlookuppathlong(mFat->f, mTarget, npath, &dir, &index) != 0) {
            return std::unexpected(Error("mkdir: long pre path does not exist"));
        }
        const auto nd = fatentrygetfirstcluster(dir, index, fatbits(mFat->f));
        dir = fatclusterread(mFat->f, nd);
        free(npath);
    } else {
        dir = mDirectory;
        index = mIndex;
    }

    // does the path already exist
    const auto target = dir->n;
    npath = fatstoragepathlong(name.c_str());
    // does the path already exist
    if (fatlookupfilelong(mFat->f, target, npath, &dir, &index) == 0) {
        free(npath);
        if (failIfExists) {
            return std::unexpected(Error("mkdir: path already exists"));
        }
        return {};
    }

    fatcreatefilepathlong(mFat->f, target, npath, &dir, &index);

    auto ret = mkdirFinalize(dir, index, target);

    free(npath);

    return std::move(ret);
}

Result<void> FatDirectory::mkdirFinalize(unit* dir, int index, int32_t target)
{
    fatentrysetattributes(dir, index, 0x10);
    auto next = fatclusterfindfreebetween(mFat->f, mFirst, mLast, -1);
    if (next == FAT_ERR) {
        return std::unexpected(Error("mkdir: long filesystem full"));
    }
    fatentrysetfirstcluster(dir, index, fatbits(mFat->f), next);
    fatsetnextcluster(mFat->f, next, FAT_EOF);

    auto cluster = fatclustercreate(mFat->f, next);
    for (index = 0; index < cluster->size / 32; ++index) {
        fatentryzero(cluster, index);
    }

    fatentrysetshortname(cluster, 0, DOTFILE);
    fatentrysetfirstcluster(cluster, 0, fatbits(mFat->f), next);
    fatentrysetattributes(cluster, 0, 0x10);

    fatentrysetshortname(cluster, 1, DOTDOTFILE);
    fatentrysetfirstcluster(cluster, 1, fatbits(mFat->f), target == mRoot ? 0 : target);
    fatentrysetattributes(cluster, 1, 0x10);

    return {};
}

Result<void> FatDirectory::mkdir(std::filesystem::path name, Recursive recursive)
{
    if (!name.is_relative()) {
        return std::unexpected(Error("mkdir: Only relative paths are supported"));
    }

    char* npath;
    // first, check if each part of the path is valid
    std::vector<std::pair<std::filesystem::path, bool>> parts;
    bool isLong = false;
    for (const auto& part : name) {
        if (fatinvalidname(part.c_str()) == 0) {
            // valid short
            npath = fatstoragename(part.c_str());
            parts.push_back({std::filesystem::path(npath), isLong});
            free(npath);
        } else if (fatinvalidnamelong(part.c_str()) == 0) {
            npath = fatstoragenamelong(part.c_str());
            // valid long
            isLong = true;
            parts.push_back({std::filesystem::path(npath), true});
            free(npath);
        } else {
            return std::unexpected(Error("mkdir: Invalid directory name"));
        }
    }

    if (parts.size() > 1 && recursive == Recursive::No) {
        return std::unexpected(Error("mkdir: Recursive is required for multiple mkdir parts"));
    }

    std::filesystem::path currentPath;
    std::size_t idx = 0;
    const std::size_t size = parts.size();
    for (const auto& part : parts) {
        const bool failIfExists = (idx == size - 1) ? true : false;
        if (part.second) {
            // long name
            auto result = mkdirLong(currentPath, part.first, failIfExists);
            if (!result) {
                return result;
            }
        } else {
            // short name
            auto result = mkdirShort(currentPath, part.first, failIfExists);
            if (!result) {
                return result;
            }
        }
        currentPath /= part.first;
        ++idx;
    }

    return {};
}

Result<FatDirectory::FatEntry> FatDirectory::openEntry(std::filesystem::path name, OpenFileMode mode, Recursive lookInSubDirs, const char* descr)
{
    char* npath;
    unit* dir = mDirectory;
    int index = mIndex;
    unit* longdir = nullptr;
    int longindex = 0;
    bool created = false, isLong = false;

    struct {
        int(*invalidshort)(const char*);
        int(*lookupshort)(fat*, int32_t, const char*, unit**, int*);
        int(*invalidlong)(const char*);
        int(*lookuplong)(fat*, int32_t, char*, unit**, int*, unit**, int*);
    } ptrs;

    if (lookInSubDirs == Recursive::No) {
        ptrs.invalidshort = fatinvalidname;
        ptrs.lookupshort = fatlookupfile;
        ptrs.invalidlong = fatinvalidnamelong;
        ptrs.lookuplong = fatlookupfilelongboth;
    } else {
        ptrs.invalidshort = fatinvalidpath;
        ptrs.lookupshort = fatlookuppath;
        ptrs.invalidlong = fatinvalidpathlong;
        ptrs.lookuplong = fatlookuppathlongboth;
    }

    if (ptrs.invalidshort(name.c_str()) == 0) {
        // short file name

        const auto target = dir->n;
        npath = fatstoragepath(name.c_str());
        // does the path already exist
        if (ptrs.lookupshort(mFat->f, target, npath, &dir, &index) != 0) {
            // file does not exist, create if mode is write
            if (mode & OpenFileMode::Write) {
                if (fatcreatefile(mFat->f, target, npath, &dir, &index) != 0) {
                    return std::unexpected(Error("{}: Failed to create file: {}", descr, name));
                }
                fatreferencesettarget(mFat->f, dir, index, 0, FAT_UNUSED);
                fatentrysetattributes(dir, index, 0x20);
                created = true;
            } else {
                return std::unexpected(Error("{}: File does not exist: {}", descr, name));
            }
        }
    } else if (ptrs.invalidlong(name.c_str()) == 0) {
        // long file name

        const auto target = dir->n;
        npath = fatstoragepathlong(name.c_str());
        // does the path already exist
        if (ptrs.lookuplong(mFat->f, target, npath, &dir, &index, &longdir, &longindex) != 0) {
            // file does not exist, create if mode is write
            if (mode & OpenFileMode::Write) {
                if (fatcreatefilepathlongboth(mFat->f, target, npath, &dir, &index, &longdir, &longindex) != 0) {
                    return std::unexpected(Error("{}: Failed to create file: {}", descr, name));
                }
                fatreferencesettarget(mFat->f, dir, index, 0, FAT_UNUSED);
                fatentrysetattributes(dir, index, 0x20);
                created = isLong = true;
            } else {
                return std::unexpected(Error("{}: File does not exist: {}", descr, name));
            }
        }
    } else {
        return std::unexpected(Error("{}: Invalid file name: {}", descr, name));
    }

    std::filesystem::path shortname, longname;

    char shortnamebuf[13];
    fatshortnametostring(shortnamebuf, &ENTRYPOS(dir, index, 0));
    shortname = shortnamebuf;

    if (isLong) {
        longname = npath;
    } else {
        free(npath);
        int res = fatlongnext(mFat->f, &dir, &index, &longdir, &longindex, &npath);
        if (res & FAT_LONG_ALL) {
            longname = npath;
        }
    }
    free(npath);

    if (longdir == nullptr) {
        longdir = dir;
        longindex = index;
    }

    return FatEntry { dir, longdir, index, longindex, created, std::move(shortname), std::move(longname) };
}

Result<void> FatDirectory::chdir(std::filesystem::path name)
{
    auto maybefentry = openEntry(name, OpenFileMode::Read, Recursive::Yes, "chdir");
    if (!maybefentry.has_value()) {
        return std::unexpected(std::move(maybefentry).error());
    }
    auto fentry = std::move(maybefentry).value();
    assert(!fentry.created);
    if (!fatreferenceisdirectory(fentry.dir, fentry.index, 0)) {
        return std::unexpected(Error("chdir: Can't chdir into a non-directory: '{}'", name));
    }

    mDirectory = fentry.dir;
    mIndex = fentry.index;
    mTarget = fentry.dir->n;
    mShort = std::move(fentry.shortname);
    mLong = std::move(fentry.longname);

    return {};
}

Result<void> FatDirectory::rm(unit* dir, int index, unit* longdir, int longindex)
{
    auto next = fatentrygetfirstcluster(dir, index, fatbits(mFat->f));
    for (auto cl = next; cl != FAT_EOF && cl != FAT_UNUSED; cl = next) {
        next = fatgetnextcluster(mFat->f, cl);
        fatsetnextcluster(mFat->f, cl, FAT_UNUSED);
    }

    if (longdir != nullptr && (longdir != dir || longindex != index)) {
        if (fatdeletelong(mFat->f, longdir, longindex) != 0) {
            fatflush(mFat->f);
            return std::unexpected(Error("rm: Failed to delete long name"));
        }
    }
    return {};
}

Result<void> FatDirectory::rm(std::filesystem::path name)
{
    auto maybefentry = openEntry(name, OpenFileMode::Read, Recursive::Yes, "rm");
    if (!maybefentry.has_value()) {
        return std::unexpected(std::move(maybefentry).error());
    }
    auto fentry = std::move(maybefentry).value();
    assert(!fentry.created);
    if (fatreferenceisdirectory(fentry.dir, fentry.index, 0)) {
        return std::unexpected(Error("rm: Can't remove directory (use rmdir): {}", name));
    }
    auto result = rm(fentry.dir, fentry.index, fentry.longdir, fentry.longindex);
    fatentrydelete(fentry.dir, fentry.index);
    fatflush(mFat->f);
    return std::move(result);
}

Result<void> FatDirectory::rmdir(std::filesystem::path name, Force force, Recursive recursive)
{
    auto maybefentry = openEntry(name, OpenFileMode::Read, Recursive::Yes, "rmdir");
    if (!maybefentry.has_value()) {
        return std::unexpected(std::move(maybefentry).error());
    }
    auto fentry = std::move(maybefentry).value();
    assert(!fentry.created);
    if (!fatreferenceisdirectory(fentry.dir, fentry.index, 0)) {
        return std::unexpected(Error("rmdir: Can't remove a non-directory (use rm): {}", name));
    }

    auto removeEntry = [&](unit* dir, int index) -> Result<void> {
        // remove all references to this directory
        struct ToRemove
        {
            std::vector<std::tuple<unit*, int>> files;
            std::vector<std::tuple<unit*, int>> dirs;
        } toremove = {};

        auto removerefs = [](fat* /*f*/,
                             unit* directory, int index, int32_t /*previous*/,
                             unit* /*startdirectory*/, int /*startindex*/, int32_t /*startprevious*/,
                             unit* /*dirdirectory*/, int /*dirindex*/, int32_t /*dirprevious*/,
                             int direction, void *user) -> int {
            if (direction == 0) {
                auto toremove = static_cast<ToRemove *>(user);
                if (fatentryisdirectory(directory, index)) {
                    if (!fatentryisdotfile(directory, index)) {
                        toremove->dirs.emplace_back(directory, index);
                    }
                } else {
                    toremove->files.emplace_back(directory, index);
                }
                return FAT_REFERENCE_ORIG | FAT_REFERENCE_RECUR | FAT_REFERENCE_ALL;
            }
            return 0;
        };

        assert(!fatentryisdotfile(dir, index));

        // collect all files to remove
        fatreferenceexecute(mFat->f, dir, index, 0, removerefs, &toremove);

        assert(!toremove.files.empty() || !toremove.dirs.empty());

        if (force == Force::No && toremove.files.size() > 0) {
            return std::unexpected(Error("rmdir: Directory not empty, has file ({})", toremove.files.size()));
        }
        // dirs == 1 means only our current entry is present
        if (recursive == Recursive::No && toremove.dirs.size() > 1) {
            return std::unexpected(Error("rmdir: Directory not empty, has directory ({})", toremove.dirs.size() - 1));
        }

        for (const auto& file : toremove.files) {
            rm(std::get<0>(file), std::get<1>(file), nullptr, 0);
            fatdeletelong(mFat->f, std::get<0>(file), std::get<1>(file));
            fatentrydelete(std::get<0>(file), std::get<1>(file));
        }
        for (const auto& file : toremove.dirs) {
            rm(std::get<0>(file), std::get<1>(file), nullptr, 0);
            fatdeletelong(mFat->f, std::get<0>(file), std::get<1>(file));
            fatentrydelete(std::get<0>(file), std::get<1>(file));
        }

        return {};
    };

    auto result = removeEntry(fentry.dir, fentry.index);
    fatflush(mFat->f);
    return std::move(result);
}

Result<void> FatDirectory::rename(std::filesystem::path src, std::filesystem::path dst)
{
}

Result<std::shared_ptr<File>> FatDirectory::openFile(std::filesystem::path name, OpenFileMode mode)
{
    auto maybefentry = openEntry(name, mode, Recursive::Yes, "openFile");
    if (!maybefentry.has_value()) {
        return std::unexpected(std::move(maybefentry).error());
    }
    auto fentry = std::move(maybefentry).value();

    if (!fentry.created) {
        // file exists, check and truncate if needed
        if (fatreferenceisdirectory(fentry.dir, fentry.index, 0)) {
            return std::unexpected(Error("openFile: Existing file is a directory: {}", name));
        }
        if (mode & OpenFileMode::Truncate) {
            if (mode & OpenFileMode::Read) {
                return std::unexpected(Error("openFile: Can't both truncate and read"));
            }
            // truncate
            int32_t next = fatentrygetfirstcluster(fentry.dir, fentry.index, fatbits(mFat->f));
            for (int32_t cl = next; cl != FAT_EOF && cl != FAT_UNUSED; cl = next) {
                next = fatgetnextcluster(mFat->f, cl);
                fatsetnextcluster(mFat->f, cl, FAT_UNUSED);
            }

            fatreferencesettarget(mFat->f, fentry.dir, fentry.index, 0, FAT_UNUSED);
            fatentrysetsize(fentry.dir, fentry.index, 0);
        }
    }

    return std::shared_ptr<FatFile>(new FatFile(mFat, std::move(fentry.shortname), std::move(fentry.longname), fentry.dir, fentry.index));
}

Result<void> FatDirectory::copy(std::filesystem::path src, std::filesystem::path dst)
{
    // just a convenience wrapper around the file api for now, could possibly be optimized
    auto maybesrcfile = openFile(src, OpenFileMode::Read);
    if (!maybesrcfile.has_value()) {
        return std::unexpected(std::move(maybesrcfile).error());
    }
    auto maybedstfile = openFile(dst, OpenFileMode::Write | OpenFileMode::Truncate);
    if (!maybedstfile.has_value()) {
        return std::unexpected(std::move(maybedstfile).error());
    }

    auto srcfile = std::move(maybesrcfile).value();
    auto dstfile = std::move(maybedstfile).value();

    auto rem = srcfile->size();
    while (rem > 0) {
        auto readsize = std::min<std::size_t>(rem, 32768);

        auto data = srcfile->read(readsize);
        if (!data.has_value()) {
            return std::unexpected(std::move(data).error());
        }

        auto writeresult = dstfile->write(data.value());
        if (!writeresult.has_value()) {
            return std::unexpected(std::move(writeresult).error());
        }
        rem -= readsize;
    }

    return {};
}

const std::filesystem::path& FatDirectory::shortPath() const
{
    return mShort;
}

const std::filesystem::path& FatDirectory::longPath() const
{
    return mLong.empty() ? mShort : mLong;
}

} // namespace flippy
