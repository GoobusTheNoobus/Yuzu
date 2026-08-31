/*
    Yuzu is a compiled programming language
    Copyright (C) 2026  GoobusTheNoobus

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include <chrono>
#include <iostream>
#include <iterator>
#include <random>

namespace yuzu {

constexpr const char* ERROR_MESSAGES[] = {"Nice try. Just try harder next time",
                                          "SONION :sob:",
                                          "Bro, what is this???",
                                          "Please reconsider your life choices.",
                                          "Yuzu has decided this is unacceptable.",
                                          "Interesting thought process there!",
                                          "No, just no.",
                                          "Who let bro write code",
                                          "Did you lose a bet or something?",
                                          "I'm telling a trusted adult",
                                          "Respectfully what the fuck",
                                          "Please go outside",
                                          "This is disappointing of you (as your father)",
                                          "i could explain the problem, but just look at ts",
                                          "Error detected, opinion rejected",
                                          "my cat walking across keyboard can do better"};

inline void raise_error(const std::string& msg) {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    std::cerr << "\033[31mERROR: " << msg << std::endl;
    std::cerr << ERROR_MESSAGES[rng() % std::size(ERROR_MESSAGES)] << "\033[0m" << std::endl;
    std::exit(1);
}

inline void raise_warning(const std::string& msg) {
    std::cerr << "\033[94mWARNING: " << msg << "\033[0m" << std::endl;
}
} // namespace yuzu