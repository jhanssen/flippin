#include "CopyCommand.h"
#include <Directory.h>

using namespace flippy;

int CopyCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> cargs)
{
    const int fexecute = FilesystemCommand::execute(std::move(path), format, std::move(cargs));
    if (fexecute != 0) {
        return fexecute;
    }

    const auto& a = args();
    if (a.size() != 2) {
        fmt::print(stderr, "{}: requires two arguments\n", name());
        return 1;
    }

    const auto& dir = directory();
    auto res = dir->copy(a[0], a[1]);
    if (!res) {
        fmt::print(stderr, "{}: cannot copy file: {}\n", name(), res.error().message());
        return 1;
    }

    fmt::print("{}: copied file, {} -> {}\n", name(), a[0], a[1]);

    return 0;
}

std::vector<std::string> CopyCommand::syntax() const
{
    return {
        "copy <source> <destination>",
        "  Copies a file inside of the image"
    };
}
