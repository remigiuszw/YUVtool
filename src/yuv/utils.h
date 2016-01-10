/* 
 * Copyright 2015 Dominik Wójt
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
const int bits_in_byte = 8;
using Index = std::int_fast64_t;
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
template<typename TRatio>
struct Ratio_to_double
{
    using Ratio = TRatio;
    static constexpr double value = double(Ratio::num) / Ratio::den;
};
/*----------------------------------------------------------------------------*/
} /* YUV_tool */

#endif // UTILS_H
