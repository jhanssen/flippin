#include "FatFile.h"
#include <cassert>

namespace flippy {

FatFile::FatFile(std::shared_ptr<FatFat> fat, std::filesystem::path shortp, std::filesystem::path longp, unit* directory, int index)
    : mFat(std::move(fat)), mShort(std::move(shortp)), mLong(std::move(longp)), mDirectory(directory), mIndex(index)
{
    mSize = fatentrygetsize(directory, index);
}

FatFile::~FatFile()
{
}

const std::filesystem::path& FatFile::shortPath() const
{
    return mShort;
}

const std::filesystem::path& FatFile::longPath() const
{
    return mLong.empty() ? mShort : mLong;
}

Result<std::size_t> FatFile::size() const
{
    return mSize;
}

Result<std::vector<uint8_t>> FatFile::read(std::size_t size) const
{
    if (!mFat) {
        return std::unexpected(Error("File '{}' is closed", longPath()));
    }

    if (mCluster == 0) {
        mCluster = fatentrygetfirstcluster(mDirectory, mIndex, fatbits(mFat->f));
    }

    const auto nowsize = fatentrygetsize(mDirectory, mIndex);
    if (nowsize != mSize) {
        return std::unexpected(Error("File size has changed since open ({} vs {})", nowsize, mSize));
    }
    if (mReadOffset >= nowsize) {
        return std::unexpected(Error("Offset is beyond the file size ({} vs {})", mReadOffset, nowsize));
    }

    auto remaining = std::min(size, nowsize - mReadOffset);

    std::vector<uint8_t> result;
    result.reserve(remaining);

    unit* cluster;
    unsigned char* data;
    int32_t cl;
    for (cl = mCluster; cl >= FAT_ROOT; cl = fatgetnextcluster(mFat->f, cl)) {
        cluster = fatclusterread(mFat->f, cl);
        data = fatunitgetdata(cluster);

        assert(mReadClusterOffset < cluster->size);
        const auto readsize = std::min<std::size_t>(remaining, cluster->size - mReadClusterOffset);
        result.resize(result.size() + readsize);
        memcpy(result.data() + result.size() - readsize, data + mReadClusterOffset, readsize);

        mReadOffset += readsize;
        if (remaining > (cluster->size - mReadClusterOffset)) {
            remaining -= (cluster->size - mReadClusterOffset);
        } else {
            // done, if remaining is the cluster size then go to the next cluster,
            // otherwise we have to continue at this cluster for the next read call
            if (remaining == (cluster->size - mReadClusterOffset)) {
                cl = fatgetnextcluster(mFat->f, cl);
                mReadClusterOffset = 0;
            } else {
                mReadClusterOffset = mReadClusterOffset + remaining;
            }
            break;
        }

        mReadClusterOffset = 0;
    }
    mCluster = cl;

    return result;
}

Result<std::size_t> FatFile::write(const std::vector<uint8_t>& data)
{
}

Result<void> FatFile::close()
{
    mFat.reset();
}

} // namespace flippy
