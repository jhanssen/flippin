#pragma once

//#include "DirectoryIterator.h"
#include "Entry.h"
#include <Flags.h>
#include <Result.h>
#include <filesystem>
#include <vector>

class Entry;
class File;

class Directory
{
public:
    enum class OpenMode
    {
        Read  = 0x1,
        Write = 0x2
    };

    virtual ~Directory() = 0;

    virtual Result<std::vector<Entry>> ls() const = 0;
    virtual Result<void> chdir(std::filesystem::path name) = 0;
    virtual Result<void> mkdir(std::filesystem::path name) = 0;
    virtual Result<void> rm(std::filesystem::path name) = 0;
    virtual Result<void> copy(std::filesystem::path name) = 0;
    virtual Result<void> rename(std::filesystem::path oldName, std::filesystem::path newName) = 0;
    virtual Result<File> open(std::filesystem::path name, OpenMode mode) = 0;

    static std::unique_ptr<Directory> root(std::filesystem::path path);

    // DirectoryIterator begin();
    // DirectoryIterator end();
    // DirectoryConstIterator begin() const;
    // DirectoryConstIterator end() const;
    // DirectoryConstIterator cbegin();
    // DirectoryConstIterator cend();

private:
    Directory() { }

private:
    // friend class DirectoryIterator;
    // friend class DirectoryConstIterator;
    friend class Entry;
};

template<>
struct FlagTraits<Directory::OpenMode>
{
    static constexpr bool isBitmask = true;
};
