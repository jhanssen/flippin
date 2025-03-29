#pragma once

#include "Error.h"
#include <expected>

namespace flippin {

template<typename T>
using Result = std::expected<T, Error>;

} // namespace flippin
