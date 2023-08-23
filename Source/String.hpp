#pragma once
#include <string>
#include <codecvt>
#include <unordered_map>

#include "Platform.hpp"
#include "TypeDefines.hpp"

#if defined(_WIN32)
#define UTF_CPP_CPLUSPLUS _MSVC_LANG
#endif
#include "../ThirdParty/utf8/utf8.h"

// ANSI Support
#define ANSI_TEXT(x) x

// WIDE Support
#define WIDE_TEXT(x) L ## x

// Utf-8 Support
#define UTF8_TEXT_CXX20(x) reinterpret_cast<const char*>(u8 ## x)
#define UTF8_TEXT_LEGACY(x) u8 ## x
#if _MSVC_LANG >= 202002L	/*CXX20*/
#define UTF8_TEXT(x) UTF8_TEXT_CXX20(x)
#else	
#define UTF8_TEXT(x) UTF8_TEXT_LEGACY(x)
#endif  // _MSVC_LANG >= 202002L

// Utf-16 Support
#define UTF16_TEXT(x) u ## x

// Utf-32 Support
#define UTF32_TEXT(x) U ## x

#if defined(SNOWY_CORE_CHAR_WIDE)
    #define STEXT(x) WIDE_TEXT(x)
#else
    #define STEXT(x) ANSI_TEXT(x)
#endif  // defined(SNOWY_CORE_CHAR_WIDE)

namespace Snowy
{
/// <summary>
/// STL String Convertor.
/// </summary>
struct StringConvertor
{
    static std::string WideToAnsi(WideStringIn wStr)
    {
        AnsiChar* buffer = Platform::WideStringToAnsi(wStr.data(), static_cast<int>(wStr.size()));
        std::string aStr(buffer);
        delete[] buffer;
        return aStr;
    }
    static std::wstring AnsiToWide(AnsiStringIn aStr)
    {
        WideChar* buffer = Platform::AnsiStringToWide(aStr.data(), static_cast<int>(aStr.size()));
        std::wstring wStr(buffer);
        delete[] buffer;
        return wStr;
    }
    static AnsiString WideToUtf8(WideStringIn wStr)
    {
        if constexpr (g_SCharCodeIsUtf16)
        {
            std::u16string tempStr(reinterpret_cast<const char16_t*>(wStr.data()));
            return utf8::utf16to8(tempStr);
        } else
        {
            std::u32string tempStr(reinterpret_cast<const char32_t*>(wStr.data()));
            return utf8::utf32to8(tempStr);
        }
    }
    static AnsiString AnsiToUtf8(AnsiStringIn aStr)
    {
        auto tempStr = AnsiToWide(aStr);
        return WideToUtf8(tempStr);
    }
};
#define WIDE_TO_ANSI(x) StringConvertor::WideToAnsi(x)
#define ANSI_TO_WIDE(x) StringConvertor::AnsiToWide(x)
#define WIDE_TO_UTF8(x) StringConvertor::WideToUtf8(x)
#define ANSI_TO_UTF8(x) StringConvertor::AnsiToUtf8(x)
#if defined(SNOWY_CORE_CHAR_WIDE)
    #define WIDE_TO_SSTR(x) x
    #define ANSI_TO_SSTR(x) StringConvertor::AnsiToWide(x)
    #define SSTR_TO_WIDE(x) x
    #define SSTR_TO_ANSI(x) StringConvertor::WideToAnsi(x)
    #define SSTR_TO_UTF8(x) StringConvertor::WideToUtf8(x)
#else
    #define WIDE_TO_SSTR(x) StringConvertor::WideToAnsi(x)
    #define ANSI_TO_SSTR(x) x
    #define SSTR_TO_WIDE(x) StringConvertor::AnsiToWide(x)
    #define SSTR_TO_ANSI(x) x
    #define SSTR_TO_UTF8(x) StringConvertor::AnsiToUtf8(x)
#endif // defined(SNOWY_CORE_CHAR_WIDE)

/// <summary>
/// SnowyCore's custom string tools.
/// </summary>
struct StringUtils
{
    template<typename T>
    static SString ToSString(T&& val)
    {
        if constexpr (g_SCharTypeIsWChar)
        {
            return std::to_wstring(std::forward<T>(val));
        } else
        {
            return std::to_string(std::forward<T>(val));
        }
    }
};

/// <summary>
/// SnowyCore's custom string wrapper, which is proxy class for string, default is std::string or std::wstring.
/// </summary>
/// <typeparam name="S"> String Type </typeparam>
/// <typeparam name="V"> String View Type </typeparam>
template<typename StrT, typename ViewT>
class TStringWrapper
{
public:
    using StringType = StrT;
    using StringViewType = ViewT;

private:
    StringType m_Data;

public:
    TStringWrapper() = default;
    TStringWrapper(const StringType& data) : m_Data(data) {}
    TStringWrapper(StringType&& data) : m_Data(std::forward<StringType>(data)) {}
    TStringWrapper& operator=(const StringType& data) { m_Data = data; return *this; }
    TStringWrapper& operator=(StringType&& data) { m_Data = std::forward<StringType>(data); return *this; }

    bool operator==(const TStringWrapper& other) const { return m_Data == other.m_Data; }

    operator StringType() const { return m_Data; }
    operator StringViewType() const { return m_Data; }

    StringType& Get() { return m_Data; }
    StringType& operator*() { return m_Data; }
    StringType* operator->() { return &m_Data; }
    const StringType& Get() const { return m_Data; }
    const StringType& operator*() const { return m_Data; }
    const StringType* operator->() const { return &m_Data; }
public:
    // TODO: String Common Method
};
// Default StringWrapper
using SStringWrapper = TStringWrapper<SString, SStringIn>;


// Text Localized
// TODO: Move it out of SnowyCore?
// ---------------------------------

//enum class ELocaleType
//{
//    English = 0,
//    Chinese
//};
//
///// <summary>
///// Localized string, english and chinese
///// </summary>
//class LocaleText
//{
//public:
//    static ELocaleType CurrentLocale;
//
//private:
//    std::unordered_map<ELocaleType, SStringWrapper> m_LocaleText;
//};

/// <summary>
/// Fixed String, can use for template param.
/// </summary>
template<typename CharT, size_t N>
struct FixedString
{
    using CharType = CharT;
    constexpr static std::size_t Length = N;

    CharT buffer[Length + 1]{};
    constexpr FixedString(const CharType* chars)
    {
        for (size_t i = 0; i != Length; ++i)
        {
            buffer[i] = chars[i];
        }
    }
    constexpr operator const CharT* () const { return buffer; }
    auto operator<=>(const FixedString&) const = default;
};
template<typename CharT, size_t N> FixedString(const CharT(&)[N]) -> FixedString<CharT, N - 1>;

template<FixedString Str>
constexpr auto operator""_fix()
{
    return Str;
}
#define WIDE_FIXED(x)  (WIDE_TEXT(x)        ## _fix)
#define UTF8_FIXED(x)  (UTF8_TEXT_LEGACY(x) ## _fix)
#define UTF16_FIXED(x) (UTF16_TEXT(x)       ## _fix)
#define UTF32_FIXED(x) (UTF32_TEXT(x)       ## _fix)
}
