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
#include <chrono>
#include <iostream>

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
        b * b * b * b * b * b * b * b,
        b * b * b * b * b * b * b * b * b,
        b * b * b * b * b * b * b * b * b * b,
        b * b * b * b * b * b * b * b * b * b * b,
        b * b * b * b * b * b * b * b * b * b * b * b,
        b * b * b * b * b * b * b * b * b * b * b * b * b,
        b * b * b * b * b * b * b * b * b * b * b * b * b * b,
        b * b * b * b * b * b * b * b * b * b * b * b * b * b * b,
        b * b * b * b * b * b * b * b * b * b * b * b * b * b * b * b
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
/* This class is designed as helper for making range based loops over
 * continuous ranges of e.g. enums or integers. */
template<typename TValue>
class Value_range
{
public:
    using Value = TValue;

    class Iterator
    {
    public:
        friend class Value_range;

    private:
        Value m_value;

    private:
        /* constructor is to be accesible only to Value_range class */
        Iterator(const Value value) :
            m_value(value)
        { }

    public:
        Value operator*() const
        {
            return m_value;
        }

        Iterator &operator++()
        {
            m_value = static_cast<Value>(static_cast<int>(m_value) + 1);
        }

        Iterator operator++(int)
        {
            const Iterator result = *this;
            ++(*this);
            return result;
        }

        bool operator!=(const Iterator &rhs) const
        {
            return m_value != rhs.m_value;
        }
    };

private:
    Iterator m_begin;
    Iterator m_end;

public:
    Value_range(const Value begin, const Value end) :
        m_begin(begin),
        m_end(end)
    { }

    Iterator begin() const
    {
        return m_begin;
    }

    Iterator end() const
    {
        return m_end;
    }
};
/*----------------------------------------------------------------------------*/
template<typename Value>
Value_range<Value> make_value_range(const Value begin, const Value end)
{
    return Value_range<Value>(begin, end);
}
/*----------------------------------------------------------------------------*/
/* This is a helper class for making range based loops over a range described by
 * a pair of iterators. */
template<typename TIterator>
class Range
{
public:
    using Iterator = TIterator;

private:
    Iterator m_begin;
    Iterator m_end;

public:
    Range(const Iterator begin, const Iterator end) :
        m_begin(begin),
        m_end(end)
    { }

    Iterator begin() const
    {
        return m_begin;
    }

    Iterator end() const
    {
        return m_end;
    }
};
/*----------------------------------------------------------------------------*/
template<typename Iterator>
Range<Iterator> make_range(const Iterator begin, const Iterator end)
{
    return Range<Iterator>(begin, end);
}
/*----------------------------------------------------------------------------*/
class Timer
{
private:
    using Clock = std::chrono::high_resolution_clock;

private:
    Clock::time_point m_start_point;
    std::string m_message;

public:
    Timer(const std::string &message) :
        m_start_point(Clock::now()),
        m_message(message)
    { }

    ~Timer()
    {
        const Clock::duration time_elapsed = Clock::now() - m_start_point;
        const double time_in_ms =
                time_elapsed.count()
                * Ratio_to_double<
                    std::ratio_divide<
                        Clock::duration::period,
                        std::milli> >::value;
        std::cerr
                << "Timer \"" << m_message << "\" : "
                << time_in_ms << " ms\n";
    }
};
/*----------------------------------------------------------------------------*/
} /* YUV_tool */

#endif // UTILS_H
