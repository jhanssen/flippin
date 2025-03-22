#pragma once

#include "Entry.h"
#include <filesystem>
#include <cstddef>

namespace flippy {

struct EntrySentinel {};

template<typename T>
class EntryIteratorBase
{
public:
    using difference_type = std::ptrdiff_t;
    using value_type = Entry;

    EntryIteratorBase() = default;

    EntryIteratorBase(const T& dir)
        : mBase(dir.path()), mEntries(dir.ls()), mIndex(0)
    {
    }

    Entry& operator*()
    {
        return mEntries[mIndex];
    }

    const Entry& operator*() const
    {
        return mEntries[mIndex];
    }

    Entry* operator->()
    {
        if (mIndex < mEntries.size()) {
            return &mEntries[mIndex];
        }
        return nullptr;
    }

    const Entry* operator->() const
    {
        if (mIndex < mEntries.size()) {
            return &mEntries[mIndex];
        }
        return nullptr;
    }

    EntryIteratorBase& operator++()
    {
        if (mIndex < mEntries.size()) {
            ++mIndex;
        }
        return *this;
    }

    EntryIteratorBase operator++(int)
    {
        EntryIteratorBase tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const EntrySentinel&) const
    {
        return mIndex >= mEntries.size();
    }

    bool operator==(const EntryIteratorBase& other) const
    {
        return mBase == other.mBase && mIndex == other.mIndex;
    }

private:
    std::filesystem::path mBase;
    std::vector<Entry> mEntries;
    std::size_t mIndex;
};

template<typename T>
struct EntryRangeBase
{
    EntryRangeBase(const T& dir)
        : mBegin(dir)
    {
    }

    auto begin() const { return mBegin; }
    auto end() const { return EntrySentinel{}; }

private:
    EntryIteratorBase<T> mBegin;
};

} // namespace flippy
