#pragma once

#include <Format.h>
#include <filesystem>
#include <string>
#include <vector>

namespace flippy {

class Command
{
public:
    Command(std::string name);
    virtual ~Command() = default;

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) = 0;
    virtual std::vector<std::string> syntax() const = 0;

    const std::string& name() const { return mName; }

private:
    std::string mName;
};

inline Command::Command(std::string name)
    : mName(std::move(name))
{
}

} // namespace flippy
