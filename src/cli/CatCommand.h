#pragma once

#include "FilesystemCommand.h"

namespace flippin {

class CatCommand : public FilesystemCommand
{
public:
    CatCommand() : FilesystemCommand("cat") {}
    virtual ~CatCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippin
