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
#ifndef ERRORS_H
#define ERRORS_H

#include <yuv/utils.h>

#include <exception>
#include <stdexcept>
#include <string>

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
class GeneralError : public std::exception
{
public:
    GeneralError(const std::string &file_name);
    const char* what() const throw();
private:
    std::string m_description;
};
/*----------------------------------------------------------------------------*/
template<typename Exception = std::runtime_error, typename Argument>
void my_assert(const bool test, const Argument &argument)
{
    if(!test)
        throw Exception(argument);
}
/*----------------------------------------------------------------------------*/
inline void check_range(Index begin, Index value, Index end)
{
    my_assert<std::out_of_range>(
            begin <= value && value < end,
            "YUVtool variable out of range");
}
/*----------------------------------------------------------------------------*/
#define MY_ASSERT(test) \
    YUV_tool::my_assert( \
        test, \
        "Assertion failed: (" # test \
        "), but the programmer was to lazy to explain what it means");
/*----------------------------------------------------------------------------*/
} /* namespace YUV_tool */

#endif // ERRORS_H
