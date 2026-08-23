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

#include "token.hpp"

namespace yuzu 
{
    std::string token_type_to_string(Token::Type type)
    {
        constexpr static const char* map[] = {
            "END_OF_FILE",
            "IDENTIFIER",

            "LITERAL_INT",
            "LITERAL_FLOAT",
            "LITERAL_STRING",
            "LITERAL_CHAR",

            "KEYWORD_EXIT",
            "KEYWORD_RETURN",
            "KEYWORD_TRUE",
            "KEYWORD_FALSE",
            "KEYWORD_LET",
            "KEYWORD_FUNC",

            "OPERATOR_PLUS",
            "OPERATOR_MINUS",
            "OPERATOR_STAR",
            "OPERATOR_SLASH",
            "OPERATOR_PERCENT",
            "OPERATOR_ASSIGNMENT",

            "LEFT_PAREN",
            "RIGHT_PAREN",
            "LEFT_BRACKET",
            "RIGHT_BRACKET",
            "LEFT_BRACE",
            "RIGHT_BRACE",
            "SEMICOLON",
            "COLON",
            "DOT",
            "COMMA"
        };

        return (int)type > (int)Token::Type::DOT ? "UNKNOWN": map[(int)type];
    }

    std::ostream& operator<<(std::ostream& os, const Token& token)
    {
        os << "{ type: " << token_type_to_string(token.type) << ", value: '" << token.value << "' }";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const std::vector<Token>& tokens)
    {
        for (Token token : tokens)
        {
            os << token << '\n';
        }

        return os;
    }
}