#pragma once

#include "FilesystemCommand.h"

namespace flippin {

class DelCommand : public FilesystemCommand
{
public:
    DelCommand() : FilesystemCommand("del") {}
    virtual ~DelCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippin
