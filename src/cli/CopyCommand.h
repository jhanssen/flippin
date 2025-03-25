#pragma once

#include "FilesystemCommand.h"

namespace flippy {

class CopyCommand : public FilesystemCommand
{
public:
    CopyCommand() : FilesystemCommand("copy") {}
    virtual ~CopyCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippy
