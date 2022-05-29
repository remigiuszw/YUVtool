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

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
constexpr std::uint32_t saturable_fixed::shift;
constexpr std::uint32_t saturable_fixed::total_bits;
constexpr std::uint32_t saturable_fixed::integer_bits;
constexpr std::uint32_t saturable_fixed::integer_sign_mask;
constexpr std::int32_t saturable_fixed::max_integer;
constexpr std::int32_t saturable_fixed::min_integer;
constexpr std::uint32_t saturable_fixed::fraction_mask;
constexpr std::uint32_t saturable_fixed::sign_mask;
constexpr std::uint32_t saturable_fixed::max_internal;
constexpr std::uint32_t saturable_fixed::min_internal;
constexpr std::uint64_t saturable_fixed::double_sign_mask;
/*----------------------------------------------------------------------------*/
} /* namespace YUV_tool */
