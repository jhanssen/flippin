#pragma once

#include <string>

class Error
{
public:
    Error(std::string message)
        : mMessage(std::move(message))
    {
    }

    const std::string& message() const { return mMessage; }
    std::string acquire() { return std::move(mMessage); }

private:
    std::string mMessage;
};
