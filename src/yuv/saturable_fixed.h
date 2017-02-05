/*
 * Copyright 2017 Dominik Wójt
 *
 * This file is part of YUVtool.
 *
 * YUVtool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YUVtool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YUVtool.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef FIXED_H
#define FIXED_H

#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <Eigen/Core>

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
class saturable_fixed
{
public:
    using internal_uint = std::uint32_t;
    using double_internal_uint = std::uint64_t;
    static constexpr std::uint32_t shift = 16;
    static constexpr std::uint32_t total_bits = 32;

    static constexpr std::uint32_t integer_bits = total_bits - shift;
    static constexpr std::uint32_t integer_sign_mask =
            static_cast<std::uint32_t>(1) << (integer_bits - 1u);
    static constexpr std::int32_t max_integer =
            integer_sign_mask - 1u;
    static constexpr std::int32_t min_integer = -integer_sign_mask;
    static_assert(max_integer == 0x7fff, "error in max integer calculation");
    static_assert(min_integer == -0x8000, "error in min integer calculation");


    static constexpr std::uint32_t fraction_mask =
            (static_cast<std::uint32_t>(1) << saturable_fixed::shift) - 1u;

    static constexpr internal_uint sign_mask =
            static_cast<internal_uint>(1) << (total_bits - 1u);
    static constexpr internal_uint max_internal =
            sign_mask - 1u;
    static constexpr internal_uint min_internal =
            sign_mask;

    static constexpr double_internal_uint double_sign_mask =
            static_cast<double_internal_uint>(1) << (2 * total_bits - 1);

private:
    internal_uint m_value;

public:
    saturable_fixed() :
        m_value(0)
    { }

    saturable_fixed(std::int32_t integer)
    {
        integer = std::min(max_integer, std::max(min_integer, integer));
        std::uint32_t unsigned_integer =
                static_cast<std::uint32_t>(integer - min_integer)
                - integer_sign_mask;
        m_value = unsigned_integer << shift;
    }

    static saturable_fixed from_integer(std::int32_t integer)
    {
        return saturable_fixed(integer);
    }

    static saturable_fixed from_internal(internal_uint x)
    {
        saturable_fixed result;
        result.m_value = x;
        return result;
    }

    std::int32_t to_int() const
    {
        return
                min_integer
                + static_cast<std::int32_t>(
                    (m_value >> shift) + integer_sign_mask);
    }

    internal_uint get_internal() const
    {
        return m_value;
    }

private:
    static double_internal_uint sign_extend_to_double(internal_uint x)
    {
        return
                static_cast<double_internal_uint>(x + sign_mask)
                - saturable_fixed::sign_mask;
    }

    friend saturable_fixed &operator+=(saturable_fixed&, saturable_fixed);
    friend saturable_fixed &operator-=(saturable_fixed&, saturable_fixed);
    friend saturable_fixed &operator*=(saturable_fixed&, saturable_fixed);
    friend saturable_fixed &operator/=(saturable_fixed&, saturable_fixed);
    friend saturable_fixed operator+(saturable_fixed, saturable_fixed);
    friend saturable_fixed operator-(saturable_fixed, saturable_fixed);
    friend saturable_fixed operator*(saturable_fixed, saturable_fixed);
    friend saturable_fixed operator/(saturable_fixed, saturable_fixed);
    friend saturable_fixed operator-(saturable_fixed);
    friend bool operator==(saturable_fixed, saturable_fixed);
    friend bool operator<(saturable_fixed, saturable_fixed);
    friend bool operator<=(saturable_fixed, saturable_fixed);
    friend bool operator>(saturable_fixed, saturable_fixed);
    friend bool operator>=(saturable_fixed, saturable_fixed);
    friend saturable_fixed sqrt(saturable_fixed);
    friend std::ostream &operator<<(std::ostream &, saturable_fixed);
    friend std::istream &operator>>(std::istream &, saturable_fixed &);
};
/*----------------------------------------------------------------------------*/
inline saturable_fixed &operator+=(
        saturable_fixed &lhs,
        const saturable_fixed rhs)
{
    lhs = lhs + rhs;
    return lhs;
}
/*----------------------------------------------------------------------------*/
inline saturable_fixed &operator-=(
        saturable_fixed &lhs,
        const saturable_fixed rhs)
{
    lhs = lhs - rhs;
    return lhs;
}
/*----------------------------------------------------------------------------*/
inline saturable_fixed &operator*=(
        saturable_fixed &lhs,
        const saturable_fixed rhs)
{
    lhs = lhs * rhs;
    return lhs;
}
/*----------------------------------------------------------------------------*/
inline saturable_fixed &operator/=(
        saturable_fixed &lhs,
        const saturable_fixed rhs)
{
    lhs = lhs / rhs;
    return lhs;
}
/*----------------------------------------------------------------------------*/
inline saturable_fixed operator+(
        const saturable_fixed lhs,
        const saturable_fixed rhs)
{
    using sf = saturable_fixed;
    using uint_t = sf::internal_uint;
    const uint_t lhs_uint = lhs.get_internal();
    const uint_t rhs_uint = rhs.get_internal();
    const uint_t sum = lhs_uint + rhs_uint;

    /* underflow */
    if ((lhs_uint & rhs_uint & ~sum) & sf::sign_mask)
        return saturable_fixed::from_internal(sf::min_internal);

    /* overflow */
    if ((~lhs_uint & ~rhs_uint & sum) & sf::sign_mask)
        return sf::from_internal(sf::max_internal);

    return sf::from_internal(sum);
}
/*----------------------------------------------------------------------------*/
inline saturable_fixed operator-(
        const saturable_fixed lhs,
        const saturable_fixed rhs)
{
    using sf = saturable_fixed;
    using uint_t = sf::internal_uint;
    const uint_t lhs_uint = lhs.get_internal();
    const uint_t rhs_uint = rhs.get_internal();
    const uint_t difference = lhs_uint - rhs_uint;

    /* underflow */
    if ((lhs_uint & ~rhs_uint & ~difference) & sf::sign_mask)
        return sf::from_internal(sf::min_internal);

    /* overflow */
    if ((~lhs_uint & rhs_uint & difference) & sf::sign_mask)
        return sf::from_internal(sf::max_internal);

    return sf::from_internal(difference);
}
/*----------------------------------------------------------------------------*/
inline saturable_fixed operator*(
        const saturable_fixed lhs,
        const saturable_fixed rhs)
{
    using sf = saturable_fixed;
    using uint_t = sf::double_internal_uint;
    const uint_t lhs_uint =
            sf::sign_extend_to_double(lhs.get_internal());
    const uint_t rhs_uint =
            sf::sign_extend_to_double(rhs.get_internal());
    const uint_t product = lhs_uint * rhs_uint;
    const uint_t max_product =
            sf::sign_extend_to_double(sf::max_internal) << sf::shift;
    const uint_t min_product =
            sf::sign_extend_to_double(sf::min_internal) << sf::shift;
    if ((max_product - product) & sf::double_sign_mask)
        return sf::from_internal(sf::max_internal);
    if ((product - min_product) & sf::double_sign_mask)
        return sf::from_internal(sf::min_internal);
    const uint_t rounded_half_up =
            product
            + (static_cast<sf::double_internal_uint>(1) << (sf::shift - 1));
    return
            sf::from_internal(
                static_cast<sf::internal_uint>(rounded_half_up >> sf::shift));
}
/*----------------------------------------------------------------------------*/
inline saturable_fixed operator/(
        const saturable_fixed lhs,
        const saturable_fixed rhs)
{
    using sf = saturable_fixed;
    using uint_t = sf::double_internal_uint;
    const uint_t lhs_uint =
            sf::sign_extend_to_double(lhs.get_internal());
    const uint_t rhs_uint =
            sf::sign_extend_to_double(rhs.get_internal());
    if (rhs_uint == 0)
        return sf::from_internal(0);
    const uint_t fraction =
            (lhs_uint << (2 * sf::shift)) / rhs_uint;
    const uint_t max_fraction =
            sf::sign_extend_to_double(sf::max_internal) << sf::shift;
    const uint_t min_fraction =
            sf::sign_extend_to_double(sf::min_internal) << sf::shift;
    if ((max_fraction - fraction) & sf::double_sign_mask)
        return sf::from_internal(sf::max_internal);
    if ((fraction - min_fraction) & sf::double_sign_mask)
        return sf::from_internal(sf::min_internal);
    const uint_t rounded_half_up =
            fraction
            + (static_cast<sf::double_internal_uint>(1) << (sf::shift - 1));
    return
            sf::from_internal(
                static_cast<sf::internal_uint>(rounded_half_up >> sf::shift));
}
/*----------------------------------------------------------------------------*/
inline saturable_fixed operator-(const saturable_fixed x)
{
    return saturable_fixed::from_internal(0) - x;
}
/*----------------------------------------------------------------------------*/
inline bool operator==(
        const saturable_fixed lhs,
        const saturable_fixed rhs)
{
    return lhs.m_value == rhs.m_value;
}
/*----------------------------------------------------------------------------*/
inline bool operator<(
        const saturable_fixed lhs,
        const saturable_fixed rhs)
{
    return
            lhs.m_value + saturable_fixed::sign_mask
            < rhs.m_value + saturable_fixed::sign_mask;
}
/*----------------------------------------------------------------------------*/
inline bool operator<=(
        const saturable_fixed lhs,
        const saturable_fixed rhs)
{
    return
            lhs.m_value + saturable_fixed::sign_mask
            <= rhs.m_value + saturable_fixed::sign_mask;
}
/*----------------------------------------------------------------------------*/
inline bool operator>(
        const saturable_fixed lhs,
        const saturable_fixed rhs)
{
    return
            lhs.m_value + saturable_fixed::sign_mask
            > rhs.m_value + saturable_fixed::sign_mask;
}
/*----------------------------------------------------------------------------*/
inline bool operator>=(
        const saturable_fixed lhs,
        const saturable_fixed rhs)
{
    return
            lhs.m_value + saturable_fixed::sign_mask
            >= rhs.m_value + saturable_fixed::sign_mask;
}
/*----------------------------------------------------------------------------*/
inline std::uint32_t integral_log2_floor(std::uint64_t x)
{
    std::uint32_t result = 0;
    if (x & 0xffffffff00000000ull)
    {
        result += 32;
        x >>= 32;
    }
    if (x & 0xffff0000ull)
    {
        result += 16;
        x >>= 16;
    }
    if (x & 0xff00ull)
    {
        result += 8;
        x >>= 8;
    }
    if (x & 0xf0ull)
    {
        result += 4;
        x >>= 4;
    }
    if (x & 0xcull)
    {
        result += 2;
        x >>= 2;
    }
    if (x & 0x2ull)
    {
        result += 1;
        x >>= 1;
    }
    return result;
}
/*----------------------------------------------------------------------------*/
inline std::uint64_t integral_sqrt_floor(const std::uint64_t x)
{
    const std::uint32_t log2_x = integral_log2_floor(x);
    const std::uint64_t result_min =
            static_cast<std::uint64_t>(1) << (log2_x / 2u);
    std::uint64_t result = (result_min << 1u) - 1u;
    while (result * result > x)
    {
        result = (result + x / result) / 2u;
    }
    return result;
}
/*----------------------------------------------------------------------------*/
inline saturable_fixed sqrt(saturable_fixed x)
{
    using sf = saturable_fixed;
    using uint_t = sf::double_internal_uint;
    const uint_t x_uint =
            sf::sign_extend_to_double(x.get_internal());
    if (x_uint & sf::double_sign_mask)
        return sf::from_internal(0);
    return
            sf::from_internal(
                static_cast<sf::internal_uint>(
                    integral_sqrt_floor(x_uint << sf::shift)));
}
/*----------------------------------------------------------------------------*/
inline std::ostream &operator<<(std::ostream &stream, saturable_fixed x)
{
    stream
            << x.to_int()
            << '+' << (x.get_internal() & saturable_fixed::fraction_mask)
            << '/' << (saturable_fixed::fraction_mask + 1u);
    return stream;
}
/*----------------------------------------------------------------------------*/
inline std::string to_string(const saturable_fixed x)
{
    std::stringstream ss;
    ss << x;
    return ss.str();
}
/*----------------------------------------------------------------------------*/
template<typename T>
T from_string(const std::string &string);
/*----------------------------------------------------------------------------*/
template<>
inline uint32_t from_string<uint32_t>(const std::string& string)
{
    if (string.size() > 9)
        throw std::runtime_error("string is to long");
    const auto c_to_i =
            [](const char c) -> char
            {
                switch (c)
                {
                case '0': return 0;
                case '1': return 1;
                case '2': return 2;
                case '3': return 3;
                case '4': return 4;
                case '5': return 5;
                case '6': return 6;
                case '7': return 7;
                case '8': return 8;
                case '9': return 9;
                default:
                    throw std::runtime_error("unexpected character");
                }
            };
    uint64_t result = 0;
    for (uint32_t i = 0; i < string.size(); ++i)
    {
        result = (result * 10) + c_to_i(string[i]);
    }
    if (result >= (static_cast<uint64_t>(1) << 32))
        throw std::runtime_error("result out of range");
    return static_cast<uint32_t>(result);
}
/*----------------------------------------------------------------------------*/
inline std::istream &operator>>(std::istream &stream, saturable_fixed &x)
{
    std::string s;
    stream >> s;
    int sign = 1;
    size_t int_begin = 0;
    size_t int_end = 0;
    if (s[0] == '+')
    {
        s[0] = ' ';
        ++int_begin;
    }
    else if (s[0] == '-')
    {
        s[0] = ' ';
        sign = - 1;
        ++int_begin;
    }
    size_t plus_pos = s.find('+');
    if (plus_pos == std::string::npos)
        int_end = s.size();
    else
        int_end = plus_pos;

    /* integer part */
    {
        const uint32_t int_val =
                from_string<uint32_t>(s.substr(int_begin, int_end));
        const uint32_t max = saturable_fixed::integer_sign_mask;
        if (
                (sign == 1 && int_val >= max)
                || (sign == -1 && int_val > max))
            throw std::runtime_error("value out of range");
        x = saturable_fixed::from_integer(sign * static_cast<int32_t>(int_val));
    }

    /* fractional part */
    if (plus_pos != std::string::npos)
    {
        auto div_pos = s.find('/');
        if (div_pos == std::string::npos || div_pos < plus_pos)
            throw std::runtime_error(
                    "invalid saturable fixed read from std::istream");
        std::uint32_t numerator =
                from_string<uint32_t>(s.substr(plus_pos, div_pos));
        std::uint32_t denominator =
                from_string<uint32_t>(s.substr(div_pos + 1, s.size()));
        if (
                denominator == 0
                || (saturable_fixed::fraction_mask + 1u) % denominator != 0
                || numerator >= denominator)
            throw std::runtime_error(
                    "invalid saturable fixed read from std::istream");
        saturable_fixed fraction =
                saturable_fixed::from_internal(
                    numerator
                    * ((saturable_fixed::fraction_mask + 1u) / denominator));
        x += fraction;
    }

    return stream;
}
/*----------------------------------------------------------------------------*/
} /* namespace YUV_tool */
/*----------------------------------------------------------------------------*/
namespace Eigen {
/*----------------------------------------------------------------------------*/
template<> struct NumTraits<YUV_tool::saturable_fixed>
{
    typedef YUV_tool::saturable_fixed Real;
    typedef YUV_tool::saturable_fixed NonInteger;
    typedef YUV_tool::saturable_fixed Nested;
    typedef YUV_tool::saturable_fixed Literal;

    enum
    {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 10,
        MulCost = 30
    };

    static inline int digits10()
    {
        return 0;
    }
};
/*----------------------------------------------------------------------------*/
} /* namespace Eigen */

#endif /* FIXED_H */
