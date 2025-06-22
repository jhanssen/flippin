#pragma once

#include "FilesystemCommand.h"

namespace flippin {

class TypeCommand : public FilesystemCommand
{
public:
    TypeCommand() : FilesystemCommand("type") {}
    virtual ~TypeCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippin
