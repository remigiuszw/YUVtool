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
