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

#include <yuv/Fixed.h>
#include <yuv/utils.h>

#include <stdexcept>
#include <iostream>
#include <random>

using namespace YUV_tool;

void integral_log2_floor_test()
{
    const auto error =
            std::runtime_error("integral_log2_floor failed");

    if (integral_log2_floor(0) != 0)
        throw error;
    if (integral_log2_floor(1) != 0)
        throw error;
    if (integral_log2_floor(2) != 1)
        throw error;
    if (integral_log2_floor(3) != 1)
        throw error;
    if (integral_log2_floor(4) != 2)
        throw error;
    if (integral_log2_floor(0xffffffffffffffffull) != 63)
        throw error;

    std::mt19937_64 random_gen;
    const uint32_t inputs_per_test = 10000000;
    std::vector<std::uint64_t> input(inputs_per_test);
    for (auto &x : input)
        x = random_gen();

    {
        Timer timer("integral_log2_floor_test");

        for (auto &x : input)
        {
            const auto result = integral_log2_floor(x);
            const bool is_correct =
                    (result == 0 && x == 0)
                    || (
                        (1ull << result) <= x
                        && x <= (((1ull << result) << 1u) - 1u));
            if (!is_correct)
                throw error;
        }
    }
}

void integral_sqrt_floor_test()
{
    const auto error =
            std::runtime_error("integral_sqrt_floor failed");

    if (integral_sqrt_floor(0) != 0)
        throw error;
    if (integral_sqrt_floor(1) != 1)
        throw error;
    if (integral_sqrt_floor(2) != 1)
        throw error;
    if (integral_sqrt_floor(3) != 1)
        throw error;
    if (integral_sqrt_floor(4) != 2)
        throw error;
    if (integral_sqrt_floor(8) != 2)
        throw error;
    if (integral_sqrt_floor(9) != 3)
        throw error;
    if (integral_sqrt_floor(0xffffffffffffffffull) != 0xffffffffull)
        throw error;

    std::mt19937_64 random_gen;
    const uint32_t inputs_per_test = 10000000;
    std::vector<std::uint64_t> input(inputs_per_test);
    for (auto &x : input)
        x = random_gen();

    {
        Timer timer("integral_sqrt_floor_test");

        for (auto &x : input)
        {
            const auto result = integral_sqrt_floor(x);
            const bool is_correct =
                    result * result <= x
                    && x <= (result + 1u) * (result + 1u) - 1u;
            if (!is_correct)
                throw error;
        }
    }
}

int main() try
{
    integral_log2_floor_test();
    integral_sqrt_floor_test();

    std::cout << "all tests passed!" << std::endl;

    return 0;
}
catch(std::runtime_error &e)
{
    std::cerr << "runtime error caught: " << e.what() << std::endl;
    return -1;
}
catch(...)
{
    std::cerr << "unknown exception caught: " << std::endl;
    return -1;
}
