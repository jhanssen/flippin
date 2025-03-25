#include "Slashes.h"
#include <algorithm>

namespace flippy {

std::string convertSlashes(std::string input)
{
    // replace all '\\' with '/'
    std::replace(input.begin(), input.end(), '\\', '/');
    return std::move(input);
}

std::vector<std::string> convertSlashes(std::vector<std::string> input)
{
    std::vector<std::string> result;
    result.reserve(input.size());
    for (auto& str : input) {
        result.push_back(std::move(convertSlashes(std::move(str))));
    }
    return std::move(result);
}

} // namespace flippy
