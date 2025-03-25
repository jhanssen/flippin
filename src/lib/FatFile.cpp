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

std::size_t FatFile::size() const
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
        return std::unexpected(Error("File read beyond the file size ({} vs {})", mReadOffset, nowsize));
    }

    auto remaining = std::min(size, nowsize - mReadOffset);

    std::vector<uint8_t> result;
    result.reserve(remaining);

    unit* cluster;
    unsigned char* data;
    int32_t cl;
    for (cl = mCluster; cl != FAT_EOF && cl != FAT_UNUSED && cl != FAT_BAD; cl = fatgetnextcluster(mFat->f, cl)) {
        cluster = fatclusterread(mFat->f, cl);
        data = fatunitgetdata(cluster);
        const auto clusterSize = cluster->size;

        assert( static_cast<int>(mReadClusterOffset) < clusterSize);
        const auto readsize = std::min<std::size_t>(remaining, clusterSize - mReadClusterOffset);
        result.resize(result.size() + readsize);
        memcpy(result.data() + result.size() - readsize, data + mReadClusterOffset, readsize);

        fatunitdelete(&mFat->f->clusters, cl);

        mReadOffset += readsize;
        if (remaining > (clusterSize - mReadClusterOffset)) {
            remaining -= (clusterSize - mReadClusterOffset);
        } else {
            // done, if remaining is the cluster size then go to the next cluster,
            // otherwise we have to continue at this cluster for the next read call
            if (remaining == (clusterSize - mReadClusterOffset)) {
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
    if (!mFat) {
        return std::unexpected(Error("File '{}' is closed", longPath()));
    }

    const auto nowsize = fatentrygetsize(mDirectory, mIndex);
    // skip to last cluster of the file
    if (mWriteOffset == 0 && mCluster == 0) {
        const auto bpc = static_cast<std::size_t>(fatbytespercluster(mFat->f));
        auto remsize = nowsize;

        mCluster = fatentrygetfirstcluster(mDirectory, mIndex, fatbits(mFat->f));
        for (int32_t cl = mCluster; cl != FAT_EOF && cl != FAT_UNUSED && cl != FAT_BAD; cl = fatgetnextcluster(mFat->f, cl)) {
            mCluster = cl;
            if (remsize >= bpc) {
                remsize -= bpc;
                mWriteOffset += bpc;
            } else {
                // we're somewhere in the last cluster
                mWriteClusterOffset = remsize;
                mWriteOffset += remsize;
                break;
            }
        }
    }

    unit* cluster;
    unsigned char* cdata;
    auto remaining = data.size();
    auto dataoffset = 0;
    int32_t cl = mCluster;
    for (;;) {
        if (mWriteClusterOffset > 0) {
            // use previous cluster
            cluster = fatclusterread(mFat->f, cl);
            cdata = fatunitgetdata(cluster);
        } else {
            // create new cluster
            auto next = fatclusterfindfree(mFat->f);
            if (next == FAT_ERR) {
                // file system full
                fatentrysetsize(mDirectory, mIndex, mWriteOffset);
                fatflush(mFat->f);
                return std::unexpected(Error("Unable to find free cluster"));
            }

            cluster = fatclustercreate(mFat->f, next);
            if (cluster == NULL)
                cluster = fatclusterread(mFat->f, next);
            if (cluster == NULL) {
                // error creating cluster
                fatentrysetsize(mDirectory, mIndex, mWriteOffset);
                fatflush(mFat->f);
                return std::unexpected(Error("Unable to create new or get existing cluster"));
            }

            fatreferencesettarget(mFat->f, mDirectory, mIndex, cl, next);
            fatsetnextcluster(mFat->f, next, FAT_EOF);

            cdata = fatunitgetdata(cluster);
            cl = next;
        }

        // copy into cluster data
        const auto csize = std::min(cluster->size - mWriteClusterOffset, remaining);
        memcpy(cdata + mWriteClusterOffset, data.data() + dataoffset, csize);
        mWriteOffset += csize;

        cluster->dirty = 1;
        fatunitwriteback(cluster);

        fatunitdelete(&mFat->f->clusters, cl);

        if (remaining > csize) {
            remaining -= csize;
            dataoffset += csize;
        } else {
            if (remaining < csize) {
                mWriteClusterOffset += remaining;
            } else {
                mWriteClusterOffset = 0;
            }
            break;
        }

        mWriteClusterOffset = 0;
    }
    mCluster = cl;

    assert(mWriteOffset == nowsize + data.size());
    fatentrysetsize(mDirectory, mIndex, nowsize + data.size());
    fatflush(mFat->f);

    return {};
}

Result<void> FatFile::close()
{
    if (!mFat) {
        return std::unexpected(Error("File '{}' is already closed", longPath()));
    }
    mFat.reset();
    return {};
}

} // namespace flippy
