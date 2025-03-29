#pragma once

#include "Result.h"
#include <filesystem>
#include <vector>
#include <cstdint>

namespace flippin {
namespace fileio {

Result<std::vector<uint8_t>> read(const std::filesystem::path& path);
Result<void> write(const std::filesystem::path& path, const std::vector<uint8_t>& data);

}} //namespace flippin::file
