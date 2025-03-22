#include "FatFile.h"

namespace flippy {

FatFile::FatFile(std::shared_ptr<FatFat> fat, std::filesystem::path shortp, std::filesystem::path longp, unit* directory, int index)
    : mFat(std::move(fat)), mShort(std::move(shortp)), mLong(std::move(longp)), mDirectory(directory), mIndex(index)
{
}

FatFile::~FatFile()
{
}

Result<std::filesystem::path> FatFile::shortPath() const
{
    return mShort;
}

Result<std::filesystem::path> FatFile::longPath() const
{
    return mLong.empty() ? mShort : mLong;
}

Result<std::size_t> FatFile::size() const
{
}

Result<std::vector<uint8_t>> FatFile::read(std::size_t offset, std::size_t size) const
{
}

Result<std::size_t> FatFile::write(std::size_t offset, const std::vector<uint8_t>& data)
{
}

Result<void> FatFile::close()
{
}

} // namespace flippy
