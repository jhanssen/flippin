#pragma once

#include <memory>
#include <variant>
#include <cassert>

namespace flippy {

class File;
class Directory;

class Entry
{
public:
    Entry(std::variant<std::shared_ptr<File>, std::shared_ptr<Directory>>&& entry);
    ~Entry();

    Entry(Entry&&) = default;
    Entry(const Entry&) = delete;

    Entry& operator=(Entry&&) = default;
    Entry& operator=(const Entry&) = delete;

    bool isFile() const
    {
        return std::holds_alternative<std::shared_ptr<File>>(mEntry);
    }

    bool isDirectory() const
    {
        return std::holds_alternative<std::shared_ptr<Directory>>(mEntry);
    }

    File* asFile()
    {
        assert(isFile());
        return std::get<std::shared_ptr<File>>(mEntry).get();
    }

    Directory* asDirectory()
    {
        assert(isDirectory());
        return std::get<std::shared_ptr<Directory>>(mEntry).get();
    }

    std::shared_ptr<File> acquireFile()
    {
        assert(isFile());
        return std::move(std::get<std::shared_ptr<File>>(mEntry));
    }

    std::shared_ptr<Directory> acquireDirectory()
    {
        assert(isDirectory());
        return std::move(std::get<std::shared_ptr<Directory>>(mEntry));
    }

private:
    std::variant<std::shared_ptr<File>, std::shared_ptr<Directory>> mEntry;
};

} // namespace flippy
