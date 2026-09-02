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
#include "core/core.hpp"
#include "frontend/lexer/token.hpp"
#include <algorithm>
#include <string>
#include <vector>

namespace yuzu {

class Lexer {
  public:
    Lexer(const std::string& str) : source(str) {}
    std::vector<Token> tokenize();

  private:
    std::string source;
    usize pos = 0;
    std::vector<Token>* p_tokens;

    inline char peek(isize offset = 0) const {
        const auto index = std::clamp<isize>(isize(pos) + offset, 0, source.size() - 1);

        return source.at(index);
    }
    inline char next() {
        ++pos;
        return peek(-1);
    }
    inline bool end() const { return pos >= source.size(); }
    inline bool check(char c) const { return peek() == c; }
    inline bool match(char c) {
        if (end() || peek() != c) {
            return false;
        }

        next();
        return true;
    }

    void tokenize_word(); // word as in identifier/keyword
    void tokenize_num();
    void tokenize_symbol();
    void tokenize_string();
    void tokenize_char();
    void skip_comment();
};
} // namespace yuzu