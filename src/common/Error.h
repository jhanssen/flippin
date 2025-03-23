#pragma once

#include <iterator>
#include <string>
#include <fmt/format.h>
#include <fmt/std.h>

namespace flippy {

class Error
{
public:
    Error(std::string message)
        : mMessage(std::move(message))
    {
    }
    template<typename... Args, std::enable_if_t<sizeof...(Args) != 0, int> = 0>
    Error(fmt::format_string<Args...> format_str, Args&&... args)
    {
        fmt::format_to(std::back_inserter(mMessage), format_str, std::forward<Args>(args)...);
    }

    const std::string& message() const { return mMessage; }
    std::string acquire() { return std::move(mMessage); }

private:
    std::string mMessage;
};

} // namespace flippy
