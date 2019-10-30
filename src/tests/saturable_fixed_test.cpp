/*
 * Copyright 2017 Dominik Wójt
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

#include <yuv/saturable_fixed.h>
#include <yuv/utils.h>

#include <gtest/gtest.h>
#include <Eigen/Dense>
#include <stdexcept>
#include <iostream>
#include <random>

using namespace YUV_tool;
/*----------------------------------------------------------------------------*/
TEST(saturable_fixed_test, simple_arithmetic_test)
{
    const saturable_fixed a = saturable_fixed(1) / 32;
    const saturable_fixed b = saturable_fixed(-1) / 32;

    EXPECT_EQ(a - a, saturable_fixed(0));
    EXPECT_EQ(a + b, saturable_fixed(0));
    EXPECT_EQ(1 / a, saturable_fixed(32));
    EXPECT_EQ(1 / b, saturable_fixed(-32));
    EXPECT_EQ(-b - a, saturable_fixed(0));
    EXPECT_EQ(-a / b, saturable_fixed(1));
    EXPECT_EQ(b * 32, saturable_fixed(-1));
}
/*----------------------------------------------------------------------------*/
TEST(saturable_fixed_test, integral_log2_floor_test)
{
    EXPECT_EQ(integral_log2_floor(0), 0);
    EXPECT_EQ(integral_log2_floor(1), 0);
    EXPECT_EQ(integral_log2_floor(2), 1);
    EXPECT_EQ(integral_log2_floor(3), 1);
    EXPECT_EQ(integral_log2_floor(4), 2);
    EXPECT_EQ(integral_log2_floor(0xffffffffffffffffull), 63);

    std::mt19937_64 random_gen;
    const uint32_t inputs_per_test = 1000000;
    std::vector<std::uint64_t> input(inputs_per_test);
    for (auto &x : input)
        x = random_gen();

    {
        Timer timer("integral_log2_floor_test");

        for (auto &x : input)
        {
            const auto result = integral_log2_floor(x);
            EXPECT_TRUE(
                    (result == 0 && x == 0)
                    || (
                        (1ull << result) <= x
                        && x <= (((1ull << result) << 1u) - 1u)));
        }
    }
}
/*----------------------------------------------------------------------------*/
inline std::uint64_t integral_sqrt_floor_bisection(const std::uint64_t x)
{
    if (x == 0)
        return 0;
    const std::uint32_t log2_x = integral_log2_floor(x);
    std::uint64_t result_min = 1ull << (log2_x / 2u);
    std::uint64_t result_max = (result_min << 1u) - 1u;

    if (result_min * result_min == x)
        return result_min;
    if (result_max * result_max <= x)
        return result_max;

    while (result_max - result_min > 1u)
    {
        std::uint64_t average = result_min + ((result_max - result_min) >> 1u);
        if (average * average > x)
            result_max = average;
        else if (average * average < x)
            result_min = average;
        else
            return average;
    }
    return result_min;
}
/*----------------------------------------------------------------------------*/
TEST(saturable_fixed_test, integral_sqrt_floor_test)
{
    EXPECT_EQ(integral_sqrt_floor(0), 0);
    EXPECT_EQ(integral_sqrt_floor(1), 1);
    EXPECT_EQ(integral_sqrt_floor(2), 1);
    EXPECT_EQ(integral_sqrt_floor(3), 1);
    EXPECT_EQ(integral_sqrt_floor(4), 2);
    EXPECT_EQ(integral_sqrt_floor(8), 2);
    EXPECT_EQ(integral_sqrt_floor(9), 3);
    EXPECT_EQ(integral_sqrt_floor(0xffffffffffffffffull), 0xffffffffull);

    std::mt19937_64 random_gen;
    const uint32_t inputs_per_test = 1000000;
    std::vector<std::uint64_t> input(inputs_per_test);
    for (auto &x : input)
        x = random_gen();

    input[0] = 0;
    input[1] = 1;
    input[2] = 2;
    input[3] = 3;
    input[4] = 4;
    input[5] = 8;
    input[6] = 9;
    input[7] = 0xffffffffffffffffull;
    input[8] = 0xffffffffull * 0xffffffffull;
    input[9] = 0xffffffffull * 0xffffffffull - 1u;
    input[10] = 0xfffffffeull * 0xfffffffeull;
    input[11] = 0xfffffffeull * 0xfffffffeull - 1u;
    input[12] = 0xfffffffdull * 0xfffffffdull;
    input[13] = 0xfffffffdull * 0xfffffffdull - 1u;
    input[14] = 0xfffffffcull * 0xfffffffcull;
    input[15] = 0xfffffffcull * 0xfffffffcull - 1u;

    {
        Timer timer("integral_sqrt_floor_test");

        for (auto &x : input)
        {
            const auto result = integral_sqrt_floor(x);
            EXPECT_TRUE(
                    result * result <= x
                    && x <= (result + 1u) * (result + 1u) - 1u);
        }
    }

    {
        Timer timer("integral_sqrt_floor_bisection_test");

        for (auto &x : input)
        {
            const auto result = integral_sqrt_floor_bisection(x);
            EXPECT_TRUE(
                    result * result <= x
                    && x <= (result + 1u) * (result + 1u) - 1u);
        }
    }

    {
        Timer timer("integral_sqrt_floor_std_test");

        for (auto &x : input)
        {
            /* C++ standard does not require this to work. */
            const auto result =
                    static_cast<std::uint64_t>(
                        std::sqrt(static_cast<long double>(x)));
            EXPECT_TRUE(
                    result * result <= x
                    && x <= (result + 1u) * (result + 1u) - 1u);
        }
    }
}
/*----------------------------------------------------------------------------*/
TEST(saturable_fixed_test, color_transform_last_step)
{
    const auto output_in_encoded_range {saturable_fixed {16} / 255};
    const Index output_width {8};
    const Index quantized_output =
        (output_in_encoded_range * ((1 << output_width) - 1)).to_int_round();
    EXPECT_EQ(quantized_output, 16);
}
/*----------------------------------------------------------------------------*/
TEST(saturable_fixed_test, simple_inversion)
{
    using sf = saturable_fixed;

    Eigen::Matrix<sf, 2, 2> M;
    M(0, 0) = sf::from_integer(1) / sf::from_integer(2);
    M(1, 1) = sf::from_integer(2);
    EXPECT_EQ(M * M.inverse(), (Eigen::Matrix<sf, 2, 2>::Identity()));
}
/*----------------------------------------------------------------------------*/
