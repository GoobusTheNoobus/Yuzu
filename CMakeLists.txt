# =========================================================================
#    Yuzu is a compiled programming language
#    Copyright (C) 2026  GoobusTheNoobus
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.
# =========================================================================

cmake_minimum_required(VERSION 3.20)
project(Yuzu)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(yuzu
    src/main.cpp
    src/lexer.cpp
    src/token.cpp
)

target_include_directories(yuzu PRIVATE src)

# object generation
target_compile_options(yuzu PRIVATE
    # debug flags
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>

    # release flags
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Release>:-flto>
    $<$<CONFIG:Release>:-DNDEBUG>
)

# linking
target_link_options(yuzu PRIVATE
    $<$<CONFIG:Release>:-flto>
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)