#include "Filesystem.h"
#include "FatDirectory.h"
extern "C" {
#include <llfat.h>
}

namespace flippy {

static inline Format formatFromPath(std::filesystem::path path)
{
    if (path.extension() == ".fdi") {
        return Format::FDI;
    } else if (path.extension() == ".hdm") {
        return Format::HDM;
    }
    return Format::Auto;
}

Result<std::shared_ptr<Directory>> Filesystem::root(std::filesystem::path path, Format format)
{
    // open existing file
    if (format == Format::Auto) {
        format = formatFromPath(path);
    }

    std::size_t offset;
    switch (format) {
    case Format::Auto:
        return std::unexpected(Error("Could not determine format"));
    case Format::FDI:
        offset = 4096; // FDI offset
        break;
    case Format::HDM:
        offset = 0; // HDM offset
        break;
    }

    char* filename = strdup(path.c_str());
    fat* f = fatopen(filename, offset);
    if (f == nullptr) {
        free(filename);
        return std::unexpected(Error("Could not open FAT filesystem"));
    }
    int res = fatcheck(f);
    if (res) {
        free(filename);
        return std::unexpected(Error("Invalid FAT filesystem"));
    }
    return std::make_shared<FatDirectory>(std::make_shared<FatFat>(f));
}

Result<std::shared_ptr<Directory>> Filesystem::create(std::filesystem::path path, std::size_t size, Format format)
{
    // create new file
}

} // namespace flippy
