#pragma once

#include "Directory.h"
#include "FatFat.h"
#include <memory>
#include <cstdint>
#include <llfat.h>

namespace flippy {

class FatDirectory : public Directory
{
public:
    FatDirectory(std::shared_ptr<FatFat> fat);
    virtual ~FatDirectory() override;

    virtual Result<std::vector<Entry>> ls() const override;
    virtual Result<void> chdir(std::filesystem::path name) override;
    virtual Result<void> mkdir(std::filesystem::path name) override;
    virtual Result<void> rm(std::filesystem::path name) override;
    virtual Result<void> copy(std::filesystem::path name) override;
    virtual Result<void> rename(std::filesystem::path oldName, std::filesystem::path newName) override;
    virtual Result<std::unique_ptr<File>> open(std::filesystem::path name, OpenMode mode) override;
    virtual Result<std::filesystem::path> path() const override;

private:
    FatDirectory(std::shared_ptr<FatFat> fat, const std::filesystem::path& path, int32_t target = FAT_ERR);

private:
    std::vector<Entry> buildEntries(unit* startDir, int startIndex) const;

private:
    std::shared_ptr<FatFat> mFat;
    int32_t mTarget, mFirst, mLast;
    unit* mDirectory = nullptr;
    int mIndex = 0;
};

} // namespace flippy
