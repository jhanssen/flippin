#pragma once

#include "FilesystemCommand.h"

namespace flippin {

class MdCommand : public FilesystemCommand
{
public:
    MdCommand() : FilesystemCommand("md") {}
    virtual ~MdCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippin
