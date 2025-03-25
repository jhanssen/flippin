#pragma once

#include "FilesystemCommand.h"

namespace flippy {

class RdCommand : public FilesystemCommand
{
public:
    RdCommand() : FilesystemCommand("rd") {}
    virtual ~RdCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippy
