#pragma once
#include <type_traits>

namespace Snowy
{
/// <summary>
/// copy from vulkan-hpp
/// </summary>
template <typename FlagBitsType>
struct FlagTraits
{
    static constexpr bool IsBitmask = false;
};
template <typename BitType>
class Flags
{
public:
    using MaskType = typename std::underlying_type_t<BitType>;

    // constructors
    constexpr Flags() noexcept : m_Mask(0) {}

    constexpr Flags(BitType bit) noexcept : m_Mask(static_cast<MaskType>(bit)) {}

    constexpr Flags(Flags<BitType> const& rhs) noexcept = default;

    constexpr explicit Flags(MaskType flags) noexcept : m_Mask(flags) {}

    // relational operators
    auto operator<=>(Flags<BitType> const&) const = default;

    // logical operator
    constexpr bool operator!() const noexcept
    {
        return !m_Mask;
    }

    // bitwise operators
    constexpr Flags<BitType> operator&(Flags<BitType> const& rhs) const noexcept
    {
        return Flags<BitType>(m_Mask & rhs.m_Mask);
    }

    constexpr Flags<BitType> operator|(Flags<BitType> const& rhs) const noexcept
    {
        return Flags<BitType>(m_Mask | rhs.m_Mask);
    }

    constexpr Flags<BitType> operator^(Flags<BitType> const& rhs) const noexcept
    {
        return Flags<BitType>(m_Mask ^ rhs.m_Mask);
    }

    constexpr Flags<BitType> operator~() const noexcept
    {
        return Flags<BitType>(m_Mask ^ FlagTraits<BitType>::AllFlags.m_Mask);
    }

    // assignment operators
    constexpr Flags<BitType>& operator=(Flags<BitType> const& rhs) noexcept = default;

    constexpr Flags<BitType>& operator|=(Flags<BitType> const& rhs) noexcept
    {
        m_Mask |= rhs.m_Mask;
        return *this;
    }

    constexpr Flags<BitType>& operator&=(Flags<BitType> const& rhs) noexcept
    {
        m_Mask &= rhs.m_Mask;
        return *this;
    }

    constexpr Flags<BitType>& operator^=(Flags<BitType> const& rhs) noexcept
    {
        m_Mask ^= rhs.m_Mask;
        return *this;
    }

    // cast operators
    explicit constexpr operator bool() const noexcept
    {
        return !!m_Mask;
    }

    explicit constexpr operator MaskType() const noexcept
    {
        return m_Mask;
    }

    constexpr MaskType Value() const noexcept
    {
        return m_Mask;
    }

private:
    MaskType m_Mask;
};

// bitwise operators
template <typename BitType>
constexpr Flags<BitType> operator&(BitType bit, Flags<BitType> const& flags) noexcept
{
    return flags.operator&(bit);
}

template <typename BitType>
constexpr Flags<BitType> operator|(BitType bit, Flags<BitType> const& flags) noexcept
{
    return flags.operator|(bit);
}

template <typename BitType>
constexpr Flags<BitType> operator^(BitType bit, Flags<BitType> const& flags) noexcept
{
    return flags.operator^(bit);
}

// bitwise operators on BitType
template <typename BitType, typename std::enable_if<FlagTraits<BitType>::IsBitmask, bool>::type = true>
inline constexpr Flags<BitType> operator&(BitType lhs, BitType rhs) noexcept
{
    return Flags<BitType>(lhs) & rhs;
}

template <typename BitType, typename std::enable_if<FlagTraits<BitType>::IsBitmask, bool>::type = true>
inline constexpr Flags<BitType> operator|(BitType lhs, BitType rhs) noexcept
{
    return Flags<BitType>(lhs) | rhs;
}

template <typename BitType, typename std::enable_if<FlagTraits<BitType>::IsBitmask, bool>::type = true>
inline constexpr Flags<BitType> operator^(BitType lhs, BitType rhs) noexcept
{
    return Flags<BitType>(lhs) ^ rhs;
}

template <typename BitType, typename std::enable_if<FlagTraits<BitType>::IsBitmask, bool>::type = true>
inline constexpr Flags<BitType> operator~(BitType bit) noexcept
{
    return ~(Flags<BitType>(bit));
}
}
