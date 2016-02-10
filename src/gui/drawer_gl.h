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
#ifndef DRAWER_GL_H
#define DRAWER_GL_H

#include <yuv/Yuv_file.h>

#include <epoxy/gl.h>
#include <memory>

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
class Drawer_gl
{
public:
    Drawer_gl();
    ~Drawer_gl();
    void initialize(Index cache_size);
    void deinitialize();
    bool is_initialized() const;
    void attach_yuv_file(Yuv_file *yuv_file);
    void invalidate_cache();
    void draw(
            Index frame_number,
            Rectangle<Unit::pixel, Reference_point::scaled_picture>
                visible_area,
            float scale);

private:
    /* PIMPL used to remove compile time dependancy on OpenGL headers and to
     * remove long class declarations from this header. */
    class Implementation;
    std::unique_ptr<Implementation> m_implementation;
};
/*----------------------------------------------------------------------------*/
} /* namespace YUV_tool */

#endif // DRAWER_GL_H
