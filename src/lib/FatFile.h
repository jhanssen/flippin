#pragma once

#include "FatFat.h"
#include "File.h"
#include <memory>
extern "C" {
#include <llfat.h>
}

namespace flippin {

class FatDirectory;

class FatFile : public File
{
public:
    virtual ~FatFile() override;

    const std::filesystem::path& shortPath() const override;
    const std::filesystem::path& longPath() const override;
    virtual std::size_t size() const override;
    virtual Result<std::vector<uint8_t>> read(std::size_t size) const override;
    virtual Result<std::size_t> write(const std::vector<uint8_t>& data) override;
    virtual Result<void> close() override;

private:
    FatFile(std::shared_ptr<FatFat> fat, std::filesystem::path shortp, std::filesystem::path longp, unit* directory, int index);

private:
    std::shared_ptr<FatFat> mFat;
    std::filesystem::path mShort, mLong;
    unit* mDirectory = nullptr;
    int mIndex = 0;
    std::size_t mSize = 0;
    mutable int32_t mCluster = 0;
    mutable std::size_t mReadOffset = 0, mWriteOffset = 0;
    mutable std::size_t mReadClusterOffset = 0, mWriteClusterOffset = 0;

    friend class FatDirectory;
};

} // namespace flippin
