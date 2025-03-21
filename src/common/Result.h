#pragma once

#include "Error.h"
#include <expected>

template<typename T>
using Result = std::expected<T, Error>;
