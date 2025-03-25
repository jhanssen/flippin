#pragma once

#include "FilesystemCommand.h"

namespace flippy {

class DeltreeCommand : public FilesystemCommand
{
public:
    DeltreeCommand() : FilesystemCommand("deltree") {}
    virtual ~DeltreeCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippy
