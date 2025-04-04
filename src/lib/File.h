#pragma once

#include <Result.h>
#include <filesystem>
#include <limits>
#include <vector>
#include <cstddef>
#include <cstdint>

namespace flippin {

class Entry;
class Directory;

class File
{
public:
    virtual ~File() = 0;

    virtual const std::filesystem::path& shortPath() const = 0;
    virtual const std::filesystem::path& longPath() const = 0;

    virtual std::size_t size() const = 0;

    virtual Result<std::vector<uint8_t>> read(std::size_t size = std::numeric_limits<std::size_t>::max()) const = 0;
    virtual Result<std::size_t> write(const std::vector<uint8_t>& data) = 0;

    virtual Result<void> close() = 0;

protected:
    File() { }

private:
    friend class Entry;
    friend class Directory;
};

} // namespace flippin
