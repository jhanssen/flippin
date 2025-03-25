#include "Command.h"
#include "CopyCommand.h"
#include "CreateCommand.h"
#include "DirCommand.h"
#include "WriteCommand.h"
#include "Slashes.h"
#include <Args.h>
#include <File.h>
#include <Filesystem.h>
#include <Format.h>
#include <Directory.h>
#include <fmt/format.h>
#include <fmt/std.h>
#include <string>
#include <map>
#include <unordered_map>

using namespace flippy;

/*
template<typename... Args>
void print_indented(int indent, fmt::format_string<Args...> format_str, Args&&... args)
{
    fmt::print("{:{}}", "", indent);
    fmt::print(format_str, std::forward<Args>(args)...);
}
*/

static std::map<std::string, std::unique_ptr<Command>> commands;
static std::unordered_map<std::string, std::pair<Format, std::string>> formats;

static void syntax()
{
    fmt::print("Flippy: syntax: flippy --img <image> --cmd <command> [command args] [--fmt <format>]\n");
    fmt::print("Commands:\n");
    for (const auto& cmd : commands) {
        const auto& syntax = cmd.second->syntax();
        for (const auto& syn : syntax) {
            fmt::print("  {}\n", syn);
        }
        fmt::print("\n");
    }
    fmt::print("Formats:\n");
    for (const auto& fmt : formats) {
        fmt::print("  \"{}\" ({})\n", fmt.first, fmt.second.second);
    }
}

static void registerFormats()
{
    formats["FDI"] = { Format::PC98_FDI, "PC-98 FDI" };
    formats["HDM"] = { Format::PC98_HDM, "PC-98 HDM" };
    formats["144M"] = { Format::DOS_144, "DOS 1.44M" };
    formats["120M"] = { Format::DOS_120, "DOS 1.20M" };
    formats["720K"] = { Format::DOS_720, "DOS 720K" };
    formats["360K"] = { Format::DOS_360, "DOS 360K" };
}

static void registerCommand(std::unique_ptr<Command> cmd)
{
    commands[cmd->name()] = std::move(cmd);
}

int main(int argc, char** argv, char** envp)
{
    const auto args = ArgsParser::parse(
        argc, argv, envp, "FLIPPY_",
        [](const char* msg, std::size_t off, char* arg) {
            fmt::print(stderr, "Flippy: invalid argument, {} at {} in '{}'\n", msg, off, arg);
        });

    registerFormats();
    registerCommand(std::make_unique<CopyCommand>());
    registerCommand(std::make_unique<CreateCommand>());
    registerCommand(std::make_unique<DirCommand>());
    registerCommand(std::make_unique<WriteCommand>());

    std::filesystem::path image = std::filesystem::path(args.value<std::string>("img"));
    if (image.empty()) {
        fmt::print(stderr, "Flippy: missing image (--img <image>)\n");
        syntax();
        return 1;
    }

    const std::string& cmdname = args.value<std::string>("cmd");
    if (cmdname.empty()) {
        fmt::print(stderr, "Flippy: missing command (--cmd <command>)\n");
        syntax();
        return 1;
    }

    const auto& cmd = commands.find(cmdname);
    if (cmd == commands.end()) {
        fmt::print(stderr, "Flippy: unknown command '{}'\n", cmdname);
        syntax();
        return 2;
    };

    Format format = Format::Auto;
    const std::string& fmtname = args.value<std::string>("fmt");
    if (!fmtname.empty()) {
        const auto& fmt = formats.find(fmtname);
        if (fmt == formats.end()) {
            fmt::print(stderr, "Flippy: unknown format '{}'\n", fmtname);
            syntax();
            return 3;
        }
        format = fmt->second.first;
    }

    return cmd->second->execute(std::move(image), format, std::move(convertSlashes(args.freeforms())));

    /*
    if (args.freeformSize() < 1) {
        fmt::print(stderr, "Flippy: missing image name\n");
        return 1;
    }
    const bool write = args.has("w") || args.has("write");
    const bool remove = args.has("r") || args.has("remove");
    const bool removeDir = args.has("rd") || args.has("removedir");
    const bool nomkDir = args.has("nomkdir");
    const bool rename = args.has("ren") || args.has("rename");
    const bool create = args.has("c") || args.has("create");
    fmt::print("Flippy: opening image '{}'\n", args.freeformValue(0));
    const auto& imageName = args.freeformValue(0);

    if (create) {
        auto maybedir = Filesystem::create(imageName, Format::PC98_FDI);
        if (!maybedir.has_value()) {
            fmt::print(stderr, "Flippy: cannot create image '{}': {}\n", imageName, maybedir.error().message());
            return 1;
        }
        fmt::print("Flippy: created image '{}'\n", imageName);
        auto rootdir = std::move(maybedir).value();
        auto file = rootdir->openFile("AUTOEXEC.BAT", Directory::OpenFileMode::Create | Directory::OpenFileMode::Write | Directory::OpenFileMode::Truncate);
        if (!file.has_value()) {
            fmt::print(stderr, "Flippy: cannot open file: {}\n", file.error().message());
            //return 1;
        } else {
            auto res = file.value()->write(std::vector<uint8_t>({'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n'}));
            if (!res.has_value()) {
                fmt::print(stderr, "Flippy: cannot write file: {}\n", res.error().message());
                //return 1;
            } else {
                fmt::print("Wrote to file\n");
                file.value()->close();

                file = rootdir->openFile("AUTOEXEC.BAT", Directory::OpenFileMode::Read);
                auto data = file.value()->read(6);//);//0, 1024);
                if (!data.has_value()) {
                    fmt::print(stderr, "Flippy: cannot read file: {}\n", data.error().message());
                    //return 1;
                } else {
                    fmt::print("File content: {}\n", std::string(data.value().begin(), data.value().end()));
                    //fmt::print(stderr, "GOT IT\n");
                }
            }
        }
        return 0;
    }

    auto maybedir = Filesystem::root(imageName);
    if (!maybedir.has_value()) {
        fmt::print(stderr, "Flippy: cannot open image '{}': {}\n", imageName, maybedir.error().message());
        return 1;
    }
    auto rootdir = std::move(maybedir).value();

    std::function<int(const std::shared_ptr<Directory>&)> printDir;
    int iter = 0, indent = 0;
    printDir = [&printDir, &iter, &indent, write, remove, removeDir, nomkDir, rename](const std::shared_ptr<Directory>& dir) -> int {
        if (iter++ == 1 && !nomkDir) {
            auto res = dir->mkdir("sub1foobar/sub2", Directory::Recursive::Yes);
            if (!res) {
                fmt::print(stderr, "Flippy: cannot create directory: {}\n", res.error().message());
                //return 1;
            }
        }

        auto maybeentries = dir->dir();
        if (!maybeentries.has_value()) {
            fmt::print(stderr, "Flippy: cannot list entries: {}\n", maybeentries.error().message());
            return 1;
        }
        for (auto& entry : maybeentries.value()) {
            if (entry.isFile()) {
                print_indented(indent, "File '{}'\n", entry.asFile()->longPath());
            } else if (entry.isDirectory()) {
                print_indented(indent, "Directory '{}'\n", entry.asDirectory()->longPath());

                if (entry.asDirectory()->longPath() == u8"BAKE/") {
                    if (write) {
                        auto file = entry.asDirectory()->openFile("AUTOEXEC.BAT", Directory::OpenFileMode::Write | Directory::OpenFileMode::Truncate);
                        if (!file.has_value()) {
                            fmt::print(stderr, "Flippy: cannot open file: {}\n", file.error().message());
                            //return 1;
                        } else {
                            auto res = file.value()->write(std::vector<uint8_t>({'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n'}));
                            if (!res.has_value()) {
                                fmt::print(stderr, "Flippy: cannot write file: {}\n", res.error().message());
                                //return 1;
                            } else {
                                fmt::print("Wrote to file\n");
                            }
                        }
                    } else if (rename) {
                        auto res = entry.asDirectory()->rename("AUTOEXEC.BAT", "../sub1foobar/SUB2/AUTOEXEC.BAK");
                        if (!res) {
                            fmt::print(stderr, "Flippy: cannot rename file: {}\n", res.error().message());
                            //return 1;
                        } else {
                            fmt::print("Renamed file\n");
                        }
                    } else if (remove) {
                        auto res = entry.asDirectory()->rm("AUTOEXEC.BAT");
                        if (!res) {
                            fmt::print(stderr, "Flippy: cannot remove file: {}\n", res.error().message());
                            //return 1;
                        } else {
                            fmt::print("Removed file\n");
                        }
                    } else {
                        auto file = entry.asDirectory()->openFile("AUTOEXEC.BAT", Directory::OpenFileMode::Read);
                        if (!file.has_value()) {
                            fmt::print(stderr, "Flippy: cannot open file: {}\n", file.error().message());
                            //return 1;
                        } else {
                            auto data = file.value()->read();//);//0, 1024);
                            if (!data.has_value()) {
                                fmt::print(stderr, "Flippy: cannot read file: {}\n", data.error().message());
                                //return 1;
                            } else {
                                fmt::print("File content: {}\n", std::string(data.value().begin(), data.value().end()));
                                //fmt::print(stderr, "GOT IT\n");
                            }
                            /-*
                            data = file.value()->read(1025 - 38);//);//0, 1024);
                            if (!data.has_value()) {
                                fmt::print(stderr, "Flippy: cannot read file: {}\n", data.error().message());
                                //return 1;
                            } else {
                                fmt::print("File content: {}\n", std::string(data.value().begin(), data.value().end()));
                                //fmt::print(stderr, "GOT IT\n");
                            }
                            data = file.value()->read(38);//);//0, 1024);
                            if (!data.has_value()) {
                                fmt::print(stderr, "Flippy: cannot read file: {}\n", data.error().message());
                                //return 1;
                            } else {
                                fmt::print("File content: {}\n", std::string(data.value().begin(), data.value().end()));
                                //fmt::print(stderr, "GOT IT\n");
                            }
                            *-/
                        }
                    }
                } else if (entry.asDirectory()->longPath() == u8"KAKE/") {
                    if (removeDir) {
                        auto res = entry.asDirectory()->rmdir(u8"sub1foobar", Directory::Force::Yes, Directory::Recursive::Yes);
                        if (!res) {
                            fmt::print(stderr, "Flippy: cannot remove directory: {}\n", res.error().message());
                            //return 1;
                        } else {
                            fmt::print("Removed directory\n");
                            return 0;
                        }
                    }
                }

                indent += 2;
                printDir(entry.acquireDirectory());
                indent -= 2;
            }
        }

        return 0;
    };

    return printDir(rootdir);
    */
}
