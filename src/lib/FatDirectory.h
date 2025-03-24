#pragma once

#include "Directory.h"
#include "FatFat.h"
#include <memory>
#include <cstdint>
extern "C" {
#include <llfat.h>
}

namespace flippy {

class Filesystem;

class FatDirectory : public Directory
{
public:
    FatDirectory(std::shared_ptr<FatFat> fat);
    virtual ~FatDirectory() override;

    const std::filesystem::path& shortPath() const override;
    const std::filesystem::path& longPath() const override;
    virtual Result<std::vector<Entry>> dir() const override;
    virtual Result<void> chdir(std::filesystem::path name) override;
    virtual Result<void> mkdir(std::filesystem::path name, Recursive recursive) override;
    virtual Result<void> rmdir(std::filesystem::path name, Force force, Recursive recursive) override;
    virtual Result<void> rm(std::filesystem::path name) override;
    virtual Result<void> copy(std::filesystem::path src, std::filesystem::path dst) override;
    virtual Result<void> rename(std::filesystem::path src, std::filesystem::path dst) override;
    virtual Result<std::shared_ptr<File>> openFile(std::filesystem::path name, OpenFileMode mode) override;

private:
    FatDirectory(std::shared_ptr<FatFat> fat, std::filesystem::path shortp, std::filesystem::path longp, int32_t target);

private:
    std::vector<Entry> buildEntries(unit* startDir, int startIndex) const;
    Result<void> mkdirShort(const std::filesystem::path& currentPath, const std::filesystem::path& name, bool failIfExists);
    Result<void> mkdirLong(const std::filesystem::path& currentPath, const std::filesystem::path& name, bool failIfExists);
    Result<void> mkdirFinalize(unit* dir, int index, int32_t target);
    Result<void> rm(unit* dir, int index, unit* longdir = nullptr, int longindex = 0);

    struct FatEntry
    {
        unit *dir, *longdir;
        int index, longindex;
        bool created;

        std::filesystem::path shortname, longname;
    };
    Result<FatEntry> openEntry(std::filesystem::path name, OpenFileMode mode, Recursive lookInSubDirs, const char* descr);

private:
    std::shared_ptr<FatFat> mFat;
    int32_t mTarget, mRoot, mFirst, mLast;
    std::filesystem::path mShort, mLong;
    unit* mDirectory = nullptr;
    int mIndex = 0;

    friend class Filesystem;
};

} // namespace flippy
