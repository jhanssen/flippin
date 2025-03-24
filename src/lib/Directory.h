#pragma once

#include "DirectoryIterator.h"
#include "Entry.h"
#include <Flags.h>
#include <Result.h>
#include <filesystem>
#include <memory>
#include <vector>

namespace flippy {

class Directory
{
public:
    enum class OpenFileMode
    {
        Read     = 0x1,
        Write    = 0x2,
        Truncate = 0x4,
        Create   = 0x8
    };
    enum class Recursive
    {
        No,
        Yes
    };
    enum class Force
    {
        No,
        Yes
    };

    virtual ~Directory() = 0;

    virtual const std::filesystem::path& shortPath() const = 0;
    virtual const std::filesystem::path& longPath() const = 0;

    virtual Result<std::vector<Entry>> dir() const = 0;

    virtual Result<void> chdir(std::filesystem::path name) = 0;
    virtual Result<void> mkdir(std::filesystem::path name, Recursive recursive = Recursive::No) = 0;
    virtual Result<void> rmdir(std::filesystem::path name, Force force = Force::No, Recursive recursive = Recursive::No) = 0;

    virtual Result<void> rm(std::filesystem::path name) = 0;
    virtual Result<void> copy(std::filesystem::path src, std::filesystem::path dst) = 0;
    virtual Result<void> rename(std::filesystem::path src, std::filesystem::path dst) = 0;

    virtual Result<std::shared_ptr<File>> openFile(std::filesystem::path name, OpenFileMode mode) = 0;

protected:
    Directory() { }

private:
    friend class Entry;
    friend class Filesystem;
};

template<>
struct FlagTraits<Directory::OpenFileMode>
{
    static constexpr bool isBitmask = true;
};

using EntryIterator = EntryIteratorBase<Directory>;
using EntryRange = EntryRangeBase<Directory>;

} // namespace flippy
