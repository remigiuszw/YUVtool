#ifndef BIT_POSITION_H
#define BIT_POSITION_H

#include <yuv/utils.h>
#include <ostream>

namespace YUV_tool {

class Bit_position
{
private:
    typedef int Position_type;
    Position_type m_position;
public:
    Bit_position(Position_type position = 0) :
        m_position(position)
    { }
//------------------------------------------------------------------------------
    Bit_position(const int bytes, const int bits) :
        m_position(bytes*bits_in_byte + bits)
    { }
//------------------------------------------------------------------------------
    Position_type get_position() const
    {
        return m_position;
    }
//------------------------------------------------------------------------------
    void set_position(const Position_type position)
    {
        m_position = position;
    }
//------------------------------------------------------------------------------
    Position_type get_bytes() const
    {
        return m_position / bits_in_byte;
    }
//------------------------------------------------------------------------------
    int get_bits() const
    {
        return m_position % bits_in_byte;
    }
//------------------------------------------------------------------------------
    void set_position(const Position_type byte, const int bit)
    {
        set_position(byte * bits_in_byte + bit);
    }
};
//------------------------------------------------------------------------------
inline Bit_position &operator+=(Bit_position &a, const Bit_position &b)
{
    a.set_position(a.get_position() + b.get_position());
    return a;
}
//------------------------------------------------------------------------------
inline Bit_position &operator-=(Bit_position &a, const Bit_position &b)
{
    a.set_position(a.get_position() - b.get_position());
    return a;
}
//------------------------------------------------------------------------------
inline Bit_position operator+(const Bit_position &a, const Bit_position &b)
{
    Bit_position result = a;
    result += b;
    return result;
}
//------------------------------------------------------------------------------
inline Bit_position operator-(const Bit_position &a, const Bit_position &b)
{
    Bit_position result = a;
    result -= b;
    return result;
}
//------------------------------------------------------------------------------
inline Bit_position &operator*=(Bit_position &a, const int factor)
{
    a.set_position(a.get_position()*factor);
    return a;
}
//------------------------------------------------------------------------------
inline Bit_position operator*(const Bit_position &a, const int factor)
{
    Bit_position result = a;
    result *= factor;
    return result;
}
//------------------------------------------------------------------------------
inline Bit_position operator*(const int factor, const Bit_position &a)
{
    return a*factor;
}
//------------------------------------------------------------------------------
inline int operator/(const Bit_position &a, const Bit_position &b)
{
    return a.get_position()/b.get_position();
}
//------------------------------------------------------------------------------
inline bool operator<(const Bit_position &a, const Bit_position &b)
{
    return a.get_position() < b.get_position();
}
//------------------------------------------------------------------------------
inline bool operator>(const Bit_position &a, const Bit_position &b)
{
    return a.get_position() > b.get_position();
}
//------------------------------------------------------------------------------
inline std::ostream &operator<<(std::ostream &stream, const Bit_position &a)
{
    stream << a.get_bytes() << "bytes, " << a.get_bits() << "bits";
    return stream;
}
//------------------------------------------------------------------------------
inline Bit_position &operator++(Bit_position &a)
{
    a.set_position(a.get_position() + 1);
    return a;
}
//------------------------------------------------------------------------------
inline Bit_position operator++(Bit_position &a, const int)
{
    Bit_position tmp(a);
    ++a;
    return tmp;
}
//------------------------------------------------------------------------------
inline Bit_position &operator--(Bit_position &a)
{
    a.set_position(a.get_position() - 1);
    return a;
}
//------------------------------------------------------------------------------
inline Bit_position operator--(Bit_position &a, const int)
{
    Bit_position tmp(a);
    --a;
    return tmp;
}

} /* namespace YUV_tool */

#endif // BIT_POSITION_H
