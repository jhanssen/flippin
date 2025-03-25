#pragma once

#include "FilesystemCommand.h"

namespace flippy {

class DirCommand : public FilesystemCommand
{
public:
    DirCommand() : FilesystemCommand("dir") {}
    virtual ~DirCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippy
