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
#ifndef COORDINATES_H
#define COORDINATES_H

namespace YUV_tool {

class Coordinate_pair
{
public:
    typedef int Scalar;
private:
    Scalar m_x;
    Scalar m_y;
public:
    Coordinate_pair(const Scalar x = 0, const Scalar y = 0)
    {
        set(x, y);
    }
    void set(const Scalar x = 0, const Scalar y = 0)
    {
        set_x(x);
        set_y(y);
    }
    void set_x(const Scalar x)
    {
        m_x = x;
    }
    void set_y(const Scalar y)
    {
        m_y = y;
    }
    Scalar x() const
    {
        return m_x;
    }
    Scalar y() const
    {
        return m_y;
    }
};
//------------------------------------------------------------------------------
inline Coordinate_pair &operator+=(
        Coordinate_pair &a,
        const Coordinate_pair &b)
{
    a.set(
            a.x() + b.x(),
            a.y() + b.y());
    return a;
}
//------------------------------------------------------------------------------
inline Coordinate_pair operator+(
        Coordinate_pair a,
        const Coordinate_pair &b)
{
    return a += b;
}
//------------------------------------------------------------------------------
inline Coordinate_pair &operator-=(
        Coordinate_pair &a,
        const Coordinate_pair &b)
{
    a.set(
            a.x() - b.x(),
            a.y() - b.y());
    return a;
}
//------------------------------------------------------------------------------
inline Coordinate_pair operator-(
        Coordinate_pair a,
        const Coordinate_pair &b)
{
    return a -= b;
}
//------------------------------------------------------------------------------
inline bool operator==(
        const Coordinate_pair &a,
        const Coordinate_pair &b)
{
    return
            a.x() == b.x()
            && a.y() == b.y();
}
//------------------------------------------------------------------------------
inline bool operator!=(
        const Coordinate_pair &a,
        const Coordinate_pair &b )
{
    return !(a == b);
}
//------------------------------------------------------------------------------
enum class Unit
{
    pixel,
    macropixel,
    tile
};
//------------------------------------------------------------------------------
enum class Reference_point
{
    macropixel,
    picture
};
//------------------------------------------------------------------------------
template<Unit unit>
class Vector : private Coordinate_pair
{
public:
    using Coordinate_pair::Scalar;
    Vector(const Scalar x = 0, const Scalar y = 0) :
        Coordinate_pair(x, y)
    { }
    using Coordinate_pair::set;
    using Coordinate_pair::set_x;
    using Coordinate_pair::set_y;
    using Coordinate_pair::x;
    using Coordinate_pair::y;

    Vector &operator+=(const Vector &b)
    {
        *static_cast<Coordinate_pair *>(this) += b;
        return *this;
    }
    Vector &operator-=(const Vector &b)
    {
        *static_cast<Coordinate_pair *>(this) -= b;
        return *this;
    }
    bool operator==(const Vector &b) const
    {
        return *static_cast<const Coordinate_pair *>(this) == b;
    }
    bool operator!=(const Vector &b) const
    {
        return *static_cast<const Coordinate_pair *>(this) != b;
    }
    template<Unit coordinate_unit, Reference_point reference_point>
    friend class Coordinates;
private:
    Vector(const Coordinate_pair &coordinate_pair) :
        Coordinate_pair(coordinate_pair)
    { }
};
//------------------------------------------------------------------------------
template<Unit unit>
Vector<unit> operator+(Vector<unit> a, const Vector<unit> &b)
{
    return a += b;
}
//------------------------------------------------------------------------------
template<Unit unit>
Vector<unit> operator-(Vector<unit> a, const Vector<unit> &b)
{
    return a -= b;
}
//------------------------------------------------------------------------------
template<Unit unit, Reference_point reference_point>
class Coordinates : private Coordinate_pair
{
public:
    using Coordinate_pair::Scalar;
    Coordinates(const Scalar x = 0, const Scalar y = 0) :
        Coordinate_pair(x, y)
    { }
    using Coordinate_pair::set;
    using Coordinate_pair::set_x;
    using Coordinate_pair::set_y;
    using Coordinate_pair::x;
    using Coordinate_pair::y;

    Coordinates &operator+=(const Vector<unit> &v)
    {
        *static_cast<Coordinate_pair *>(this) += v;
        return *this;
    }
    Coordinates &operator-=(const Vector<unit> &v)
    {
        *static_cast<Coordinate_pair *>(this) -= v;
        return *this;
    }
    Vector<unit> operator-(const Coordinates &b) const
    {
        return Vector<unit>(*static_cast<const Coordinate_pair *>(this) - b);
    }
    bool operator==(const Coordinates &b) const
    {
        return *static_cast<const Coordinate_pair *>(this) == b;
    }
    bool operator!=(const Coordinates &b) const
    {
        return *static_cast<const Coordinate_pair *>(this) != b;
    }
};
//------------------------------------------------------------------------------
template<Unit unit, Reference_point reference_point>
Coordinates<unit, reference_point> operator+(
        Coordinates<unit, reference_point> a,
        const Vector<unit> &v)
{
    return a += v;
}
//------------------------------------------------------------------------------
template<Unit unit, Reference_point reference_point>
Coordinates<unit, reference_point> operator-(
        Coordinates<unit, reference_point> a,
        const Vector<unit> &v)
{
    return a -= v;
}
//------------------------------------------------------------------------------
inline Vector<Unit::pixel> cast_to_pixels(
        const Vector<Unit::macropixel> &a,
        const Vector<Unit::pixel> &macropixel_size)
{
    return Vector<Unit::pixel>(
            a.x() * macropixel_size.x(),
            a.y() * macropixel_size.y());
}
//------------------------------------------------------------------------------
inline Vector<Unit::macropixel> cast_to_macropixels(
        const Vector<Unit::pixel> &a,
        const Vector<Unit::pixel> &macropixel_size,
        Vector<Unit::pixel> &remainder)
{
    Vector<Unit::macropixel> result(
            a.x() / macropixel_size.x(),
            a.y() / macropixel_size.y());
    remainder = a - cast_to_pixels(result, macropixel_size);
    return result;
}
//------------------------------------------------------------------------------
template<Reference_point reference_point>
Coordinates<Unit::pixel, reference_point> cast_to_pixels(
        const Coordinates<Unit::macropixel, reference_point> &a,
        const Vector<Unit::pixel> &macropixel_size,
        const Coordinates<Unit::pixel, Reference_point::macropixel> &remainder =
            Coordinates<Unit::pixel, Reference_point::macropixel>(0, 0))
{
    return
            Coordinates<Unit::pixel, reference_point>(
                a.x() * macropixel_size.x(),
                a.y() * macropixel_size.y())
            + (
                remainder
                - Coordinates<Unit::pixel, Reference_point::macropixel>(0, 0));
}
//------------------------------------------------------------------------------
template<Reference_point reference_point>
Coordinates<Unit::macropixel, reference_point> cast_to_macropixels(
        const Coordinates<Unit::pixel, reference_point> &a,
        const Vector<Unit::pixel> &macropixel_size,
        Coordinates<Unit::pixel, Reference_point::macropixel> &remainder)
{
    Coordinates<Unit::macropixel, reference_point> result(
            a.x() / macropixel_size.x(),
            a.y() / macropixel_size.y());
    // TODO: write and use reference cast
    remainder =
            Coordinates<Unit::pixel, Reference_point::macropixel>(0, 0)
            + (a - cast_to_pixels(result, macropixel_size));
    return result;
}
//------------------------------------------------------------------------------
template<Unit Tunit, Reference_point Tpoint>
class Coordinate_range
{
public:
    static constexpr Unit unit = Tunit;
    static constexpr Reference_point reference_point = Tpoint;

private:
    Coordinates<unit, reference_point> m_start;
    Vector<unit> m_size;

public:
    class Iterator
    {
    private:
        const Coordinate_range &m_range;
        Coordinates<unit, reference_point> m_coordinates;

    private:
        Iterator(
                const Coordinate_range &range,
                const Coordinates<unit, reference_point> &coordinates) :
            m_range(range),
            m_coordinates(coordinates)
        { }
    public:
        Coordinates<unit, reference_point> operator*() const
        {
            return m_coordinates;
        }
        Iterator operator++()
        {
            Iterator result = *this;
            m_coordinates += Vector<unit>(1, 0);
            if(m_coordinates.x() >= m_range.m_start.x() + m_range.m_size.x())
                m_coordinates += Vector<unit>(-m_range.m_size.x(), 1);
            return result;
        }
        bool operator!=(const Iterator &other) const
        {
            return m_coordinates != other.m_coordinates;
        }
        friend class Coordinate_range;
    };

    Coordinate_range(
            const Coordinates<unit, reference_point> &start,
            const Vector<unit> &size) :
        m_start(start),
        m_size(size)
    { }
    Iterator begin() const
    {
        return Iterator(*this, m_start);
    }
    Iterator end() const
    {
        return Iterator(*this, m_start + Vector<unit>(0, m_size.y()));
    }
};

} /* namespace YUV_tool */

#endif // COORDINATES_H
