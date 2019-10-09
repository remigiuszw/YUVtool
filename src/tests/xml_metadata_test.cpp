/*
 * Copyright 2016 Dominik Wójt
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

#include <yuv/xml_metadata.h>
#include <yuv/Errors.h>

#include <gtest/gtest.h>

const std::string file_name = "test_pixel_format.xml";

TEST(xml_metadata_test, test_0)
{
    YUV_tool::store_pixel_format(YUV_tool::yuv_420p_8bit, file_name);
    const auto recovered_pixel_format =
            YUV_tool::read_pixel_format(file_name);
    EXPECT_EQ(
                recovered_pixel_format.m_macropixel_coding.m_size,
                YUV_tool::yuv_420p_8bit.m_macropixel_coding.m_size);
}
