#pragma once

#include "Command.h"
#include <Filesystem.h>

namespace flippy {

class FilesystemCommand : public Command
{
public:
    FilesystemCommand(std::string name) : Command(std::move(name)) {}
    virtual ~FilesystemCommand() override = default;

    virtual int execute(std::filesystem::path path, Format format, std::vector<std::string> args) override;

protected:
    const std::shared_ptr<Directory>& directory() const;
    const std::vector<std::string>& args() const;

private:
    std::shared_ptr<Directory> mDirectory;
    std::vector<std::string> mArgs;
};

inline const std::shared_ptr<Directory>& FilesystemCommand::directory() const
{
    return mDirectory;
}

inline const std::vector<std::string>& FilesystemCommand::args() const
{
    return mArgs;
}

} // namespace flippy
