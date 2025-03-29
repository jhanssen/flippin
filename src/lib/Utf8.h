#include <cstddef>
#include <string>
#include <limits>

namespace flippin {
namespace utf8 {

void initialize();
void finalize();

std::string wcharToUtf8(const wchar_t* str, std::size_t len = std::numeric_limits<std::size_t>::max());
std::string wcharToUtf8(const std::wstring& str);

std::wstring utf8ToWchar(const char* str, std::size_t len = std::numeric_limits<std::size_t>::max());
std::wstring utf8ToWchar(const std::string& str);

}} // namespace flippin::utf8
