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

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace yuzu {
struct Token {
    std::string value;
    enum class Type : uint8_t {
        END_OF_FILE,
        IDENTIFIER,

        LITERAL_INT,
        LITERAL_FLOAT,
        LITERAL_STRING,
        LITERAL_CHAR,

        KEYWORD_EXIT,
        KEYWORD_RETURN,
        KEYWORD_TRUE,
        KEYWORD_FALSE,
        KEYWORD_LET,
        KEYWORD_FUNC,

        OPERATOR_PLUS,  // both unary and binary
        OPERATOR_MINUS, // both unary and binary
        OPERATOR_STAR,
        OPERATOR_SLASH,
        OPERATOR_PERCENT,
        OPERATOR_ASSIGNMENT,

        OPERATOR_LOGICAL_OR,
        OPERATOR_LOGICAL_AND,
        OPERATOR_BITWISE_OR,
        OPERATOR_BITWISE_XOR,
        OPERATOR_BITWISE_AND,

        OPERATOR_COMP_EQ,
        OPERATOR_COMP_NEQ,

        OPERATOR_COMP_LESS,
        OPERATOR_COMP_MORE,
        OPERATOR_COMP_LESSEQ,
        OPERATOR_COMP_MOREEQ,

        OPERATOR_BITWISE_SHL,
        OPERATOR_BITWISE_SHR,

        // unary
        OPERATOR_LOGICAL_NOT,
        OPERATOR_BITWISE_NOT,

        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        LEFT_BRACE,
        RIGHT_BRACE,
        SEMICOLON,
        COLON,
        DOT,
        COMMA,

    } type;
};

std::string token_type_to_string(Token::Type);
std::string operator_to_string(Token::Type);

// i could define these in the header, but I need an excuse to have a token.cpp
std::ostream& operator<<(std::ostream&, const Token&);
std::ostream& operator<<(std::ostream&, const std::vector<Token>&);
} // namespace yuzu