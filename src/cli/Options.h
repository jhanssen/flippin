#pragma once

#include <string>
#include <vector>
#include <cstddef>

namespace flippin {

class Options
{
public:
    enum class CaseSensitive {
        Yes,
        No
    };

    Options(const std::vector<std::string>& args, CaseSensitive sensitive = CaseSensitive::No);

    bool has(const char opt) const;

    std::size_t nonOptions() const { return mNonOptions.size(); }
    std::size_t nonOption(std::size_t idx) const { return mNonOptions[idx]; }

private:
    std::string mOptions;
    std::vector<std::size_t> mNonOptions;
};

inline bool Options::has(const char opt) const
{
    return mOptions.find(opt) != std::string::npos;
}

} // namespace flippin
