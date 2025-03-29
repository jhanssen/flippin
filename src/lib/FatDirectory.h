#pragma once

#include "Directory.h"
#include "FatFat.h"
#include <memory>
#include <string>
#include <cstdint>
extern "C" {
#include <llfat.h>
}

namespace flippin {

class Filesystem;

class FatDirectory : public Directory
{
public:
    FatDirectory(std::shared_ptr<FatFat> fat);
    virtual ~FatDirectory() override;

    const std::filesystem::path& shortPath() const override;
    const std::filesystem::path& longPath() const override;

    virtual Result<std::vector<Entry>> dir() const override;
    virtual Result<void> cd(std::filesystem::path name) override;
    virtual Result<void> md(std::filesystem::path name, Recursive recursive) override;
    virtual Result<void> deltree(std::filesystem::path name, Force force, Recursive recursive) override;
    virtual Result<void> del(std::filesystem::path name) override;
    virtual Result<void> copy(std::filesystem::path src, std::filesystem::path dst) override;
    virtual Result<void> ren(std::filesystem::path src, std::filesystem::path dst) override;

    virtual Result<std::shared_ptr<File>> openFile(std::filesystem::path name, OpenFileMode mode) override;

private:
    FatDirectory(std::shared_ptr<FatFat> fat, std::filesystem::path shortp, std::filesystem::path longp, int32_t target);

private:
    std::vector<Entry> buildEntries(unit* startDir, int startIndex) const;
    Result<void> mdShort(const std::string& currentPath, const std::string& name, bool failIfExists);
    Result<void> mdLong(const std::wstring& currentPath, const std::wstring& name, bool failIfExists);
    Result<void> mdFinalize(unit* dir, int index, int32_t target);
    Result<void> del(unit* dir, int index, unit* longdir = nullptr, int longindex = 0);

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

} // namespace flippin
