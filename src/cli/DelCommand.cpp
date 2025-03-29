#include "DelCommand.h"
#include <Directory.h>

using namespace flippin;

int DelCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> cargs)
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
    auto res = dir->del(a[0]);
    if (!res) {
        fmt::print(stderr, "{}: cannot delete file: {}\n", name(), res.error().message());
        return 1;
    }

    fmt::print("{}: deleted file, {}\n", name(), a[0]);

    return 0;
}

std::vector<std::string> DelCommand::syntax() const
{
    return {
        "del <source>",
        "  Deletes a file inside of the image"
    };
}
