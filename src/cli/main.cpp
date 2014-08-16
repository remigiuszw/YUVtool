#include <iostream>
#include "Errors.h"

#include "TYUV_file.h"

void print_stream_info(Yuv_file &YUV_file)
{
    std::cout << "resolution: "
            << YUV_file.get_resolution().x() << " "
            << YUV_file.get_resolution().y() << '\n';
    std::cout << "frame size: " << YUV_file.get_frame_size() << '\n';
    std::cout << "frames count: " << YUV_file.get_frames_count() << '\n';
}

int main(int argc, char *argv[]) try
{
    if(argc != 4)
        throw GeneralError("usage: yuvtool_cli res_x res_y input_file.yuv");            ;

    Yuv_file input_file(argv[3]);

    Vector<Unit::pixel> resolution;
    resolution.set_x(std::atoi(argv[1]));
    resolution.set_y(std::atoi(argv[2]));
    input_file.set_resolution(resolution);

    input_file.set_pixel_format(yuv_420p_8bit);

    print_stream_info(input_file);

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
