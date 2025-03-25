#pragma once

#include "FilesystemCommand.h"

namespace flippy {

class DelCommand : public FilesystemCommand
{
public:
    DelCommand() : FilesystemCommand("copy") {}
    virtual ~DelCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippy
