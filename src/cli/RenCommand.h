#pragma once

#include "FilesystemCommand.h"

namespace flippin {

class RenCommand : public FilesystemCommand
{
public:
    RenCommand() : FilesystemCommand("ren") {}
    virtual ~RenCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippin
