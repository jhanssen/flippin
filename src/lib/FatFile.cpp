#include "FatFile.h"

namespace flippy {

FatFile::FatFile(std::shared_ptr<FatFat> fat, unit* directory, int index)
    : mFat(std::move(fat)), mDirectory(directory), mIndex(index)
{
}

FatFile::~FatFile()
{
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
