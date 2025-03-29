#pragma once

#include "Format.h"
#include <Result.h>
#include <filesystem>
#include <memory>
#include <cstddef>

namespace flippin {

class Directory;

struct FileFormat
{
    std::size_t headerByteSize;
    std::size_t dataByteSize;

    std::size_t sectorByteSize;
    std::size_t sectorsPerCluster;
    std::size_t sectorsPerTrack;
    std::size_t surfaceCount;
    std::size_t cylinderCount;
    std::size_t maxRootEntries;
};

class Filesystem
{
public:
    static void init();
    static void deinit();

    static Result<std::shared_ptr<Directory>> root(std::filesystem::path path, Format format = Format::Auto);
    static Result<std::shared_ptr<Directory>> create(std::filesystem::path path, Format format = Format::Auto);
    static Result<std::shared_ptr<Directory>> create(std::filesystem::path path, FileFormat format);

private:
    Filesystem() { }
};

} // namespace flippin
