#pragma once

#include "FilesystemCommand.h"

namespace flippin {

class WriteCommand : public FilesystemCommand
{
public:
    WriteCommand() : FilesystemCommand("write") {}
    virtual ~WriteCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippin
