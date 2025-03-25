#include "DeltreeCommand.h"
#include <Directory.h>

using namespace flippy;

int DeltreeCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> cargs)
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
    auto res = dir->deltree(a[0]);
    if (!res) {
        fmt::print(stderr, "{}: cannot deltree directory: {}\n", name(), res.error().message());
        return 1;
    }

    fmt::print("{}: deleted directory, {}\n", name(), a[0]);

    return 0;
}

std::vector<std::string> DeltreeCommand::syntax() const
{
    return {
        "deltree <source>",
        "  Deletes a directory recursively inside of the image"
    };
}
