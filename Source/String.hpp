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
#if _MSVC_LANG >= 202002L	/*CXX20*/
#define UTF8_TEXT(x) reinterpret_cast<const char*>(u8 ## x)
#else	
#define UTF8_TEXT(x) u8 ## x
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
    template<bool OutputStdU8String = false>
    static auto WideToUtf8(WideStringIn wStr) -> std::conditional_t<OutputStdU8String, Utf8String, AnsiString>
    {

    }
    template<bool OutputStdU8String = false>
    static auto AnsiToUtf8(AnsiStringIn aStr) -> std::conditional_t<OutputStdU8String, Utf8String, AnsiString>
    {

    }
private:
    //static std::wstring_convert<std::codecvt_utf8<wchar_t>> strCnv;
};
#define WIDE_TO_ANSI(x) StringConvertor::WideToAnsi(x)
#define ANSI_TO_WIDE(x) StringConvertor::AnsiToWide(x)
#define WIDE_TO_UTF8(x) StringConvertor::WideToUtf8(x)
#define ANSI_TO_UTF8(x) StringConvertor::AnsiToUtf8(x)
#if defined(SNOWY_CORE_CHAR_WIDE)
    #define WIDE_TO_SSTR(x) x
    #define ANSI_TO_SSTR(x) StringConvertor::AnsiToWide(x)
#else
    #define WIDE_TO_SSTR(x) StringConvertor::WideToAnsi(x)
    #define ANSI_TO_SSTR(x) x
#endif // defined(SNOWY_CORE_CHAR_WIDE)

/// <summary>
/// SnowyCore's custom string tools.
/// </summary>
class StringUtils
{

};

/// <summary>
/// SnowyCore's custom string class, which is std::string proxy class and add utf-8 support.
/// </summary>
template<typename SType>
class TStringWrapper
{
public:
    using WrappedType = SType;

private:
    WrappedType m_Data;

public:
    TStringWrapper() = default;
    TStringWrapper(const WrappedType& data) : m_Data(data) {}
    TStringWrapper(WrappedType&& data) : m_Data(std::forward<WrappedType>(data)) {}
    TStringWrapper& operator=(const WrappedType& data) { m_Data = data; return *this; }
    TStringWrapper& operator=(WrappedType&& data) { m_Data = std::forward<WrappedType>(data); return *this; }

    bool operator==(const TStringWrapper& other) const { return m_Data == other.m_Data; }

    operator std::string() const { return m_Data; }
    operator std::string_view() const { return m_Data; }

    WrappedType& Get() { return m_Data; }
    WrappedType& operator*() { return m_Data; }
    WrappedType* operator->() { return &m_Data; }
public:
    // TODO: String Common Method
};
// Default StringWrapper
using StringWrapper = TStringWrapper<SString>;

// Text Localized
// ---------------------------------

enum class ELocaleType
{
    English = 0,
    Chinese
};

/// <summary>
/// Localized string, english and chinese
/// </summary>
class LocaleText
{
public:
    static ELocaleType CurrentLocale;

private:
    std::unordered_map<ELocaleType, StringWrapper> m_LocaleText;
};

/// <summary>
/// Fixed String, can use for template param.
/// </summary>
template<size_t N>
struct FixedString
{
    char buffer[N + 1]{};
    constexpr FixedString(const char* s)
    {
        for (size_t i = 0; i != N; ++i)
        {
            buffer[i] = s[i];
        }
    }
    constexpr operator const char* () const { return buffer; }

    // not mandatory anymore
    auto operator<=>(const FixedString&) const = default;
};
template<size_t N> FixedString(const char(&)[N]) -> FixedString<N - 1>;
}