#pragma once

#include <memory>
#include <variant>
#include <cassert>

namespace flippin {

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

    bool isFile() const;
    bool isDirectory() const;

    File* asFile() const;
    Directory* asDirectory() const;

    std::shared_ptr<File> acquireFile();
    std::shared_ptr<Directory> acquireDirectory();

private:
    std::variant<std::shared_ptr<File>, std::shared_ptr<Directory>> mEntry;
};

inline bool Entry::isFile() const
{
    return std::holds_alternative<std::shared_ptr<File>>(mEntry);
}

inline bool Entry::isDirectory() const
{
    return std::holds_alternative<std::shared_ptr<Directory>>(mEntry);
}

inline File* Entry::asFile() const
{
    assert(isFile());
    return std::get<std::shared_ptr<File>>(mEntry).get();
}

inline Directory* Entry::asDirectory() const
{
    assert(isDirectory());
    return std::get<std::shared_ptr<Directory>>(mEntry).get();
}

inline std::shared_ptr<File> Entry::acquireFile()
{
    assert(isFile());
    return std::move(std::get<std::shared_ptr<File>>(mEntry));
}

inline std::shared_ptr<Directory> Entry::acquireDirectory()
{
    assert(isDirectory());
    return std::move(std::get<std::shared_ptr<Directory>>(mEntry));
}

} // namespace flippin
