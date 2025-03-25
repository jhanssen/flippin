#include "RdCommand.h"
#include <Directory.h>

using namespace flippy;

int RdCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> cargs)
{
    const int fexecute = FilesystemCommand::execute(std::move(path), format, std::move(cargs));
    if (fexecute != 0) {
        return fexecute;
    }

    const auto& a = args();
    if (a.size() != 1) {
        fmt::print(stderr, "{}: requires one argument\n", name());
        return 1;
    }

    const auto& dir = directory();
    auto res = dir->deltree(a[0], Directory::Force::No, Directory::Recursive::No);
    if (!res) {
        fmt::print(stderr, "{}: cannot rd directory: {}\n", name(), res.error().message());
        return 1;
    }

    fmt::print("{}: removed directory, {}\n", name(), a[0]);

    return 0;
}

std::vector<std::string> RdCommand::syntax() const
{
    return {
        "rd <source>",
        "  Removes a directory inside of the image"
    };
}
