#include "Command.h"
#include "CatCommand.h"
#include "CopyCommand.h"
#include "CreateCommand.h"
#include "DelCommand.h"
#include "DirCommand.h"
#include "ReadCommand.h"
#include "WriteCommand.h"
#include "Slashes.h"
#include <Args.h>
#include <Format.h>
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
    registerCommand(std::make_unique<CatCommand>());
    registerCommand(std::make_unique<CopyCommand>());
    registerCommand(std::make_unique<CreateCommand>());
    registerCommand(std::make_unique<DelCommand>());
    registerCommand(std::make_unique<DirCommand>());
    registerCommand(std::make_unique<ReadCommand>());
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
}
