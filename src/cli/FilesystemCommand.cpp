#include "FilesystemCommand.h"
#include <fmt/format.h>

using namespace flippin;

int FilesystemCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> args)
{
    mArgs = std::move(args);

    // open the file system
    auto maybedir = Filesystem::root(std::move(path), format);
    if (!maybedir) {
        fmt::print(stderr, "{}, error: {}\n", name(), maybedir.error().message());
        return 100;
    }

    mDirectory = std::move(maybedir).value();

    return 0;
}
