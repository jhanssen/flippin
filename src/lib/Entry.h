#pragma once

#include <memory>
#include <variant>
#include <cassert>

class File;
class Directory;

class Entry
{
public:
    Entry(std::variant<std::unique_ptr<File>, std::unique_ptr<Directory>>&& entry);
    ~Entry();

    bool isFile() const
    {
        return std::holds_alternative<std::unique_ptr<File>>(mEntry);
    }

    bool isDirectory() const
    {
        return std::holds_alternative<std::unique_ptr<Directory>>(mEntry);
    }

    File* asFile()
    {
        assert(isFile());
        return std::get<std::unique_ptr<File>>(mEntry).get();
    }

    Directory* asDirectory()
    {
        assert(isDirectory());
        return std::get<std::unique_ptr<Directory>>(mEntry).get();
    }

    std::unique_ptr<File> acquireFile()
    {
        assert(isFile());
        return std::move(std::get<std::unique_ptr<File>>(mEntry));
    }

    std::unique_ptr<Directory> acquireDirectory()
    {
        assert(isDirectory());
        return std::move(std::get<std::unique_ptr<Directory>>(mEntry));
    }

private:
    std::variant<std::unique_ptr<File>, std::unique_ptr<Directory>> mEntry;
};
