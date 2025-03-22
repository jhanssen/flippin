#pragma once

#include "Format.h"
#include <Result.h>
#include <filesystem>
#include <memory>
#include <cstddef>

namespace flippy {

class Directory;

class Filesystem
{
public:
    static Result<std::shared_ptr<Directory>> root(std::filesystem::path path, Format format = Format::Auto);
    static Result<std::shared_ptr<Directory>> create(std::filesystem::path path, std::size_t size, Format format = Format::Auto);

private:
    Filesystem() { }
};

} // namespace flippy
