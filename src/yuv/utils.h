/* 
 * Copyright 2015, 2016 Dominik Wójt
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
#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
using Byte = std::uint8_t;
using Index = std::int_fast32_t;
const Index bits_in_byte = 8;
using Length = std::int_fast32_t;
/*----------------------------------------------------------------------------*/
enum Rgba_component
{
    Rgba_component_red,
    Rgba_component_green,
    Rgba_component_blue,
    Rgba_component_alpha,
    Rgba_component_count,
    Rgba_component_rgb_count = 3
};
/*----------------------------------------------------------------------------*/
template<typename TNumber>
TNumber round_down(TNumber value,TNumber divisor)
{
    return value - value % divisor;
}
/*----------------------------------------------------------------------------*/
template<typename TNumber>
TNumber round_up(TNumber value, TNumber divisor)
{
    return round_down(value + divisor - static_cast<TNumber>(1), divisor);
}
/*----------------------------------------------------------------------------*/
template<typename TNumber>
TNumber power(const TNumber base, TNumber exponent)
{
    TNumber result = 1;
    TNumber current_power = base;
    while(exponent)
    {
        if(exponent & 1)
            result *= current_power;
        current_power *= current_power;
        exponent >>= 1;
    }
    return result;
}
/*----------------------------------------------------------------------------*/
template<Index base, typename TNumber>
TNumber cached_power(TNumber exponent)
{
    constexpr TNumber b = base;
    static const TNumber cache[] =
    {
        1,
        b,
        b * b,
        b * b * b,
        b * b * b * b,
        b * b * b * b * b,
        b * b * b * b * b * b,
        b * b * b * b * b * b * b,
        b * b * b * b * b * b * b * b
    };

    const TNumber cache_size = sizeof(cache)/sizeof(cache[0]);
    if(exponent < cache_size)
        return cache[exponent];
    else
        return power(b, exponent);
}
/*----------------------------------------------------------------------------*/
template<typename TRatio>
struct Ratio_to_double
{
    using Ratio = TRatio;
    static constexpr double value = double(Ratio::num) / Ratio::den;
};
/*----------------------------------------------------------------------------*/
} /* YUV_tool */

#endif // UTILS_H
