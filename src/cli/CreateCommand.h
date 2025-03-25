#pragma once

#include "Command.h"

namespace flippy {

class CreateCommand : public Command
{
public:
    CreateCommand() : Command("create") {}
    virtual ~CreateCommand() override {}

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;
    virtual std::vector<std::string> syntax() const override;
};

} // namespace flippy
