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
    enum class OpenMode
    {
        Read  = 0x1,
        Write = 0x2
    };

    virtual ~Directory() = 0;

    virtual Result<std::filesystem::path> path() const = 0;
    virtual Result<std::vector<Entry>> ls() const = 0;
    virtual Result<void> chdir(std::filesystem::path name) = 0;
    virtual Result<void> mkdir(std::filesystem::path name) = 0;
    virtual Result<void> rm(std::filesystem::path name) = 0;
    virtual Result<void> copy(std::filesystem::path name) = 0;
    virtual Result<void> rename(std::filesystem::path oldName, std::filesystem::path newName) = 0;
    virtual Result<std::shared_ptr<File>> open(std::filesystem::path name, OpenMode mode) = 0;

protected:
    Directory() { }

private:
    friend class Entry;
    friend class Filesystem;
};

template<>
struct FlagTraits<Directory::OpenMode>
{
    static constexpr bool isBitmask = true;
};

using EntryIterator = EntryIteratorBase<Directory>;
using EntryRange = EntryRangeBase<Directory>;

} // namespace flippy
