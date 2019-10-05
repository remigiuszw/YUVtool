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
#include <yuv/Errors.h>

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
GeneralError::GeneralError(const std::string &description) :
        m_description("general error: " + description)
{ }
/*----------------------------------------------------------------------------*/
const char *GeneralError::what() const noexcept
{
    return m_description.c_str();
}
/*----------------------------------------------------------------------------*/
} /* namespace YUV_tool */
