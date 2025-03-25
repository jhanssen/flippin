#pragma once

#include "FilesystemCommand.h"

namespace flippy {

class ReadCommand : public FilesystemCommand
{
public:
    ReadCommand() : FilesystemCommand("read") {}
    virtual ~ReadCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippy
