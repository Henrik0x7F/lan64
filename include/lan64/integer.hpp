//
// Created by henrik on 21.06.19
//

#pragma once

#include <type_traits>


template<bool ASSERT>
struct Assert
{
    static_assert(ASSERT);
};

/// Unsigned integer, at least BITS wide
template<std::size_t BITS>
using Uint = std::conditional_t<BITS == 0,
        Assert<false>,
        std::conditional_t<BITS <= 8,
            std::uint8_t,
            std::conditional_t<BITS <= 16,
                std::uint16_t,
                std::conditional_t<BITS <= 32,
                    std::uint32_t,
                    std::conditional_t<BITS <= 64,
                        std::uint64_t,
                        Assert<false>>>>>>;

/// Integer, at least BITS wide
template<std::size_t BITS>
using Int = std::conditional_t<BITS == 0,
    Assert <false>,
    std::conditional_t<BITS <= 8,
        std::int8_t,
        std::conditional_t<BITS <= 16,
            std::int16_t,
            std::conditional_t<BITS <= 32,
                std::int32_t,
                std::conditional_t<BITS <= 64,
                    std::int64_t,
                    Assert<false>>>>>>;

/// Get the unsigned version of an integer type
template<typename T>
using unsigned_t = std::conditional<std::is_integral_v<T>,
                                    std::conditional_t<std::is_unsigned_v<T>, T, Uint<sizeof(T) * 8>>,
                                    Assert <false>>;

/// Get the signed version of an integer type
template<typename T>
using signed_t = std::
conditional<std::is_integral_v<T>, std::conditional_t<std::is_signed_v<T>, T, Int<sizeof(T) * 8>>, Assert <false>>;

/// Get signed integer large enough to hold all values of enum T
template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
using matching_signed_int_t = Int<sizeof(T) * 8>;

/// Get unsigned integer large enough to hold all values of enum T
template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
using matching_unsigned_int_t = Uint<sizeof(T) * 8>;
