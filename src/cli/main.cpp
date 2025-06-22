#include "Command.h"
#include "CopyCommand.h"
#include "CreateCommand.h"
#include "DelCommand.h"
#include "DeltreeCommand.h"
#include "DirCommand.h"
#include "MdCommand.h"
#include "RdCommand.h"
#include "ReadCommand.h"
#include "RenCommand.h"
#include "TypeCommand.h"
#include "WriteCommand.h"
#include "Slashes.h"
#include <Filesystem.h>
#include <Args.h>
#include <Format.h>
#include <fmt/format.h>
#include <fmt/std.h>
#include <map>
#include <string>
#include <tuple>
#include <vector>

using namespace flippin;

/*
template<typename... Args>
void print_indented(int indent, fmt::format_string<Args...> format_str, Args&&... args)
{
    fmt::print("{:{}}", "", indent);
    fmt::print(format_str, std::forward<Args>(args)...);
}
*/

static std::map<std::string, std::unique_ptr<Command>> commands;
static std::vector<std::tuple<std::string, Format, std::string>> formats;

static void syntax()
{
    fmt::print("Syntax: flippin [--fmt <format>] --img <image> --cmd <command> [command args...] [-- [extra command args...]]\n\n");
    fmt::print("Commands:\n\n");
    for (const auto& cmd : commands) {
        const auto& syntax = cmd.second->syntax();
        for (const auto& syn : syntax) {
            fmt::print("  {}\n", syn);
        }
        fmt::print("\n");
    }
    fmt::print("Formats:\n\n");
    for (const auto& fmt : formats) {
        fmt::print("  \"{}\" ({})\n", std::get<0>(fmt), std::get<2>(fmt));
    }
    fmt::print("\n");
}

static void registerFormats()
{
    formats = {
        { "FDI", Format::PC98_FDI, "PC-98 FDI, 1.23M" },
        { "HDM", Format::PC98_HDM, "PC-98 HDM, 1.23M" },
        { "144M", Format::DOS_144, "DOS 1.44M" },
        { "120M", Format::DOS_120, "DOS 1.20M" },
        { "720K", Format::DOS_720, "DOS 720K" },
        { "360K", Format::DOS_360, "DOS 360K" }
    };
}

static void registerCommand(std::unique_ptr<Command> cmd)
{
    commands[cmd->name()] = std::move(cmd);
}

int main(int argc, char** argv, char** envp)
{
    const auto args = ArgsParser::parse(
        argc, argv, envp, "FLIPPIN_",
        [](const char* msg, std::size_t off, char* arg) {
            fmt::print(stderr, "Flippin: invalid argument, {} at {} in '{}'\n", msg, off, arg);
        });

    registerFormats();

    registerCommand(std::make_unique<CopyCommand>());
    registerCommand(std::make_unique<CreateCommand>());
    registerCommand(std::make_unique<DelCommand>());
    registerCommand(std::make_unique<DeltreeCommand>());
    registerCommand(std::make_unique<DirCommand>());
    registerCommand(std::make_unique<MdCommand>());
    registerCommand(std::make_unique<RdCommand>());
    registerCommand(std::make_unique<ReadCommand>());
    registerCommand(std::make_unique<RenCommand>());
    registerCommand(std::make_unique<TypeCommand>());
    registerCommand(std::make_unique<WriteCommand>());

    std::filesystem::path image = std::filesystem::path(args.value<std::string>("img"));
    if (image.empty()) {
        fmt::print(stderr, "Flippin: missing image (--img <image>)\n");
        syntax();
        return 1;
    }

    const std::string& cmdname = args.value<std::string>("cmd");
    if (cmdname.empty()) {
        fmt::print(stderr, "Flippin: missing command (--cmd <command>)\n");
        syntax();
        return 1;
    }

    const auto& cmd = commands.find(cmdname);
    if (cmd == commands.end()) {
        fmt::print(stderr, "Flippin: unknown command '{}'\n", cmdname);
        syntax();
        return 2;
    };

    Format format = Format::Auto;
    const std::string& fmtname = args.value<std::string>("fmt");
    if (!fmtname.empty()) {
        for (const auto& fmt : formats) {
            if (std::get<0>(fmt) == fmtname) {
                format = std::get<1>(fmt);
                break;
            }
        }
        if (format == Format::Auto) {
            fmt::print(stderr, "Flippin: unknown format '{}'\n", fmtname);
            syntax();
            return 3;
        }
    }

    Filesystem::init();

    const int ret = cmd->second->execute(std::move(image), format, convertSlashes(args.freeforms()));

    Filesystem::deinit();

    return ret;
}
