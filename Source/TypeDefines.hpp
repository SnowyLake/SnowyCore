#pragma once
#include <memory>
#include <string>
#include <type_traits>

#include "Platform.hpp"

#define DecayedTypeOf(x) std::decay_t<decltype(x)>
#define NameOf(x) #x

namespace Snowy
{
// -------------------------------------------------------------
// Smart Pointer Type Define
// -------------------------------------------------------------

template<typename T> using RawHandle = T*;
template<typename T> using UniqueHandle = std::unique_ptr<T>;
template<typename T> using SharedHandle = std::shared_ptr<T>;
template<typename T> using WeakHandle = std::weak_ptr<T>;

/// <summary>
/// It is used only as a function argument to access an assigned object, should never be used to allocate object.
/// If you want allocate ownless object, use RawHandle.
/// </summary>
template<typename T> class ObserverHandle
{
public:
    using ElementType = std::remove_reference_t<T>;

private:
    ElementType* m_Ptr = nullptr;

public:
    ObserverHandle() noexcept : m_Ptr(nullptr) {}
    ObserverHandle(ElementType* ptr) noexcept : m_Ptr(ptr) {}
    ObserverHandle(const ObserverHandle&) = default;
    ObserverHandle(ObserverHandle&&) = default;
    template<class U> ObserverHandle(ObserverHandle<U> other) noexcept
        requires std::convertible_to<typename ObserverHandle<U>::ElementType*, ElementType*>
    : m_Ptr(other.Get())
    {}
    ~ObserverHandle() = default;    // ObserverHandle has no ownership of the resource, so dtor do nothing.

    ElementType* operator->() const noexcept { return m_Ptr; }
    ElementType& operator* () const { return *m_Ptr; }
    bool operator==(const ObserverHandle& other) const { return m_Ptr == other.m_Ptr; }

    operator bool() const noexcept { return m_Ptr != nullptr; }
    operator ElementType* () const noexcept { return m_Ptr; }

    ElementType* Get() const noexcept { return m_Ptr; }
    ElementType* Release() noexcept { auto ptr = m_Ptr; m_Ptr = nullptr; return ptr; }
    void Reset(ElementType* ptr = nullptr) noexcept { m_Ptr = ptr; }
    void Swap(ObserverHandle& other) noexcept { auto ptr = m_Ptr; m_Ptr = other.m_Ptr; other.m_Ptr = ptr; }
};

template<typename T, typename... Args>
inline constexpr UniqueHandle<T> MakeUnique(Args&&... args) { return std::make_unique<T>(std::forward<Args>(args)...); }
template<typename T, typename... Args>
inline constexpr SharedHandle<T> MakeShared(Args&&... args) { return std::make_shared<T>(std::forward<Args>(args)...); }
template<typename T, typename... Args>
inline constexpr ObserverHandle<T> MakeObserver(Args&&... args) { return ObserverHandle<T>(std::forward<Args>(args)...); }


// -------------------------------------------------------------
// String Type Define
// -------------------------------------------------------------

// An ANSI char. 8-bit fixed-width representation of 7-bit chars.
using AnsiChar = char;

// A wide char. In-memory only. ?-bit fixed-width representation of the platform's natural wide char set. Could be different sizes on different platforms.
using WideChar = wchar_t;

// UTF Code char types.
using Utf8Char = char8_t;
using Utf16Char = char16_t;
using Utf32Char = char32_t;

// Char code macros
#if !defined(SNOWY_CORE_CHAR_ANSI) /*|| (defined(_WIN32) && defined(UNICODE))*/ // Win32 Api
    #define SNOWY_CORE_CHAR_WIDE
#endif // !defined(SNOWY_CORE_CHAR_ANSI)

// Char type for SnowyCore.
#if defined(SNOWY_CORE_CHAR_WIDE)
using SChar = WideChar;
using SCharUtfCode = std::conditional_t<sizeof(SChar) == sizeof(Utf16Char), Utf16Char, Utf32Char>;
#else
using SChar = AnsiChar;
using SCharUtfCode = Utf8Char;
#endif  // defined(SNOWY_CORE_CHAR_WIDE)

constexpr inline bool SCharTypeIsWChar = std::is_same_v<SChar, WideChar>;
constexpr inline bool SCharCodeIsUtf16 = std::is_same_v<SCharUtfCode, Utf16Char>;

// String Type Define
using AnsiString = std::string;
using WideString = std::wstring;
using Utf8String = std::u8string;
using Utf16String = std::u16string;
using Utf32String = std::u32string;
using SString = std::conditional_t<SCharTypeIsWChar, WideString, AnsiString>;

// Argument Wrapper
template<typename T> using In = const T&;
template<typename T> using Ref = T&;
template<typename T> using Out = T* const;
template<typename T> using ArrayIn = std::span<T>;
using AnsiStringIn = std::string_view;
using WideStringIn = std::wstring_view;
using SStringIn = std::conditional_t<SCharTypeIsWChar, WideStringIn, AnsiStringIn>;
}