#pragma once

#include <Result.h>
#include <filesystem>
#include <limits>
#include <vector>
#include <cstddef>
#include <cstdint>

namespace flippy {

class Entry;
class Directory;

class File
{
public:
    virtual ~File() = 0;

    virtual Result<std::filesystem::path> path() const = 0;
    virtual Result<std::size_t> size() const = 0;
    virtual Result<std::vector<uint8_t>> read(std::size_t offset = 0, std::size_t size = std::numeric_limits<std::size_t>::max()) const = 0;
    virtual Result<std::size_t> write(std::size_t offset, const std::vector<uint8_t>& data) = 0;
    virtual Result<void> close() = 0;

protected:
    File() { }

private:
    friend class Entry;
    friend class Directory;
};

} // namespace flippy
