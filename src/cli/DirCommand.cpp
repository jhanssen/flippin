#include "DirCommand.h"
#include <Directory.h>
#include <File.h>

using namespace flippy;

int DirCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> cargs)
{
    const int fexecute = FilesystemCommand::execute(std::move(path), format, std::move(cargs));
    if (fexecute != 0) {
        return fexecute;
    }

    const auto& a = args();
    if (a.size() > 1) {
        fmt::print(stderr, "{}: requires zero or one argument\n", name());
        return 1;
    }

    const auto& dir = directory();
    if (a.size() > 0) {
        auto maybechdir = dir->chdir(a[0]);
        if (!maybechdir) {
            fmt::print(stderr, "{}: cannot change directory: {}\n", name(), maybechdir.error().message());
            return 1;
        }
    }

    auto maybedir = dir->dir();
    if (!maybedir) {
        fmt::print(stderr, "{}: cannot list directory: {}\n", name(), maybedir.error().message());
        return 1;
    }

    const auto entries = std::move(maybedir).value();
    for (const auto& entry : entries) {
        if (entry.isDirectory()) {
            auto dir = entry.asDirectory();
            fmt::print("{}\n", dir->longPath());
        } else {
            auto file = entry.asFile();
            fmt::print("{}\n", file->longPath());
        }
    }

    return 0;
}

std::vector<std::string> DirCommand::syntax() const
{
    return {
        "dir [directory]",
        "  Lists a directory inside the image"
    };
}
