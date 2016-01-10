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
#include <iostream>
#include "Errors.h"

#include "Yuv_file.h"

void print_stream_info(YUV_tool::Yuv_file &yuv_file)
{
    std::cout << "resolution: "
            << yuv_file.get_resolution().x() << " "
            << yuv_file.get_resolution().y() << '\n';
    std::cout << "frame size: " << yuv_file.get_frame_size() << '\n';
    std::cout << "frames count: " << yuv_file.get_frames_count() << '\n';
}

const char help_string[] =
R"(Yuv tool commandline interface

usage:
yuvtool_cli COMMAND [ARGUMENTS]

valid commands:

stream_info RES_X RES_Y INPUT_YUV_FILE
    shows some information about the yuv file

convert RES_X RES_Y INPUT_420YUV_FILE OUTPUT_444RGB_FILE
    convert input 420 planar yuv file into plain non-planar 32bpp RGB file
)";


int main(int argc, char *argv[]) try
{
    using namespace YUV_tool;

    if(argc < 2)
        throw GeneralError(help_string);

    std::string command = argv[1];
    if(command == "stream_info")
    {
        if(argc != 5)
            throw GeneralError(help_string);

        Yuv_file input_file(argv[4]);

        Vector<Unit::pixel> resolution;
        resolution.set_x(std::atoi(argv[2]));
        resolution.set_y(std::atoi(argv[3]));
        input_file.set_resolution(resolution);

        input_file.set_pixel_format(yuv_420p_8bit);

        print_stream_info(input_file);
    }
    else if(command == "convert")
    {
        if(argc != 6)
            throw GeneralError(help_string);

        Yuv_file input_file(argv[4]);
        Yuv_file output_file(argv[5], std::ios::in|std::ios::out);

        Vector<Unit::pixel> resolution;
        resolution.set_x(std::atoi(argv[2]));
        resolution.set_y(std::atoi(argv[3]));

        input_file.set_resolution(resolution);
        input_file.set_pixel_format(yuv_420p_8bit);
        output_file.set_resolution(resolution);
        output_file.set_pixel_format(rgb_32bpp);
        output_file.set_frames_count(1);

        Coordinates<Unit::pixel, Reference_point::picture>
                zero_coordinate(0, 0);
        Picture_buffer input_buffer =
                input_file.extract_buffer(
                    0,
                    zero_coordinate,
                    zero_coordinate + resolution);
        Picture_buffer output_buffer =
                convert(input_buffer, rgb_32bpp);
        output_file.insert_buffer(
                    output_buffer,
                    0,
                    zero_coordinate,
                    zero_coordinate + resolution);
    }
    else
    {
        throw GeneralError(help_string);
    }

    return 0;
}
catch(std::exception &e)
{
    std::cerr << "standard exception caught: " << e.what() << std::endl;
    return 1;
}
catch(...)
{
    std::cerr << "non-standard exception caught!" << std::endl;
    return 1;
}
