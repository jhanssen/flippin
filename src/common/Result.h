#pragma once

#include "Error.h"
#include <expected>

namespace flippy {

template<typename T>
using Result = std::expected<T, Error>;

} // namespace flippy
