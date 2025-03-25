#include "WriteCommand.h"
#include <Directory.h>
#include <File.h>
#include <FileIO.h>

using namespace flippy;

int WriteCommand::execute(std::filesystem::path path, Format format, std::vector<std::string> cargs)
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

    auto maybesrc = fileio::read(a[0]);
    if (!maybesrc.has_value()) {
        fmt::print(stderr, "{}: cannot read host file: {}\n", name(), maybesrc.error().message());
        return 1;
    }
    auto src = std::move(maybesrc).value();

    const auto& dir = directory();
    auto maybedst = dir->openFile(a[1], Directory::OpenFileMode::Create | Directory::OpenFileMode::Write | Directory::OpenFileMode::Truncate);
    if (!maybedst.has_value()) {
        fmt::print(stderr, "{}: cannot open image file: {}\n", name(), maybedst.error().message());
        return 1;
    }
    auto dst = std::move(maybedst).value();

    auto maybeok = dst->write(src);
    if (!maybeok.has_value()) {
        fmt::print(stderr, "{}: cannot write image file: {}\n", name(), maybeok.error().message());
        return 1;
    }

    fmt::print("{}: wrote file, {} -> {}\n", name(), a[0], a[1]);

    return 0;
}

std::vector<std::string> WriteCommand::syntax() const
{
    return {
        "write <source> <destination>",
        "  Transfers a file from the host to an image"
    };
}
