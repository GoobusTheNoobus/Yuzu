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
#include "token.hpp"
#include <cstddef>
#include <string>
#include <vector>

namespace yuzu
{
    // class for lexical analysis
    // turns source code to token list
    class Lexer 
    {
        public:
        Lexer(const std::string& str) : source_(str) {}
        std::vector<Token> tokenize();

        private:
        std::string source_;
        size_t pos_ = 0;
        std::vector<Token>* p_tokens_;

        inline char peek() const { return end() ? '\0' : source_.at(pos_); }
        inline char next() { return source_.at(pos_++); }
        inline bool end() const { return pos_ >= source_.size(); }
        inline bool match(char c) 
        { 
            if (end() || peek() != c)
            {
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
}