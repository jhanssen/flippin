#include "Utf8.h"
#include <cstring>
#include <iconv.h>

namespace flippin {
namespace utf8 {

thread_local iconv_t iconvUtf8ToWchar;
thread_local iconv_t iconvWcharToUtf8;

void initialize()
{
    iconvUtf8ToWchar = iconv_open("WCHAR_T//TRANSLIT", "UTF-8");
    iconvWcharToUtf8 = iconv_open("UTF-8//TRANSLIT", "WCHAR_T");
}

void finalize()
{
    iconv_close(iconvUtf8ToWchar);
    iconv_close(iconvWcharToUtf8);
}

std::string wcharToUtf8(const wchar_t* str, std::size_t len)
{
    // worst case scenario
    const auto outsize = len * 4;
    std::string out(outsize, '\0');
    auto outbuf = const_cast<char*>(out.data());
    auto inbuf = reinterpret_cast<char*>(const_cast<wchar_t*>(str));
    if (len == std::numeric_limits<std::size_t>::max()) {
        len = wcslen(str);
    }
    auto inlen = len * sizeof(wchar_t);
    auto outlen = outsize;
    const auto res = iconv(iconvWcharToUtf8, &inbuf, &inlen, &outbuf, &outlen);
    if (res == static_cast<size_t>(-1)) {
        return {};
    }
    if (outlen > 0) {
        out.resize(outsize - outlen);
    }
    return out;
}

std::string wcharToUtf8(const std::wstring& str)
{
    return wcharToUtf8(str.c_str(), str.size());
}

std::wstring utf8ToWchar(const char* str, std::size_t len)
{
    // worst case scenario
    const auto outsize = len * sizeof(wchar_t);
    std::wstring out(outsize, L'\0');
    auto outbuf = reinterpret_cast<char*>(out.data());
    auto inbuf = const_cast<char*>(str);
    if (len == std::numeric_limits<std::size_t>::max()) {
        len = strlen(str);
    }
    auto inlen = len;
    auto outlen = outsize;
    const auto res = iconv(iconvUtf8ToWchar, &inbuf, &inlen, &outbuf, &outlen);
    if (res == static_cast<size_t>(-1)) {
        return {};
    }
    if (outlen > 0) {
        out.resize(outsize - outlen);
    }
    return out;
}

std::wstring utf8ToWchar(const std::string& str)
{
    return utf8ToWchar(str.c_str(), str.size());
}

}} // namespace flippin::utf8
