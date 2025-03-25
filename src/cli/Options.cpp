#include "Options.h"
#include <cctype>

using namespace flippy;

Options::Options(const std::vector<std::string>& args, CaseSensitive sensitive)
{
    for (const auto& arg : args) {
        if (arg.size() == 2 && (arg[0] == '-' || arg[0] == '/')) {
            if (sensitive == CaseSensitive::Yes) {
                mOptions.push_back(arg[1]);
            } else {
                mOptions.push_back(std::tolower(arg[1]));
                mOptions.push_back(std::toupper(arg[1]));
            }
        }
    }
}
