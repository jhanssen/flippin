#pragma once

#include <string>
#include <vector>

namespace flippy {

class Options
{
public:
    enum class CaseSensitive {
        Yes,
        No
    };

    Options(const std::vector<std::string>& args, CaseSensitive sensitive = CaseSensitive::No);

    bool has(const char opt) const;

private:
    std::string mOptions;
};

inline bool Options::has(const char opt) const
{
    return mOptions.find(opt) != std::string::npos;
}

} // namespace flippy
