#include "Options.h"
#include <cctype>

using namespace flippin;

Options::Options(const std::vector<std::string>& args, CaseSensitive sensitive)
{
    std::size_t idx = 0;
    mOptions.reserve(args.size() * (sensitive == CaseSensitive::Yes ? 1 : 2));
    mNonOptions.reserve(args.size());
    for (const auto& arg : args) {
        if (arg.size() == 2 && (arg[0] == '-' || arg[0] == '/')) {
            if (sensitive == CaseSensitive::Yes) {
                mOptions.push_back(arg[1]);
            } else {
                mOptions.push_back(std::tolower(arg[1]));
                mOptions.push_back(std::toupper(arg[1]));
            }
        } else {
            mNonOptions.push_back(idx);
        }
        ++idx;
    }
}
