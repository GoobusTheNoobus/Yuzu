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

#include "frontend/lexer/token.hpp"

namespace yuzu {
std::string token_type_to_string(Token::Type type) {
    constexpr static const char* map[] = {"END_OF_FILE",
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

                                          "OPERATOR_LOGICAL_OR",
                                          "OPERATOR_LOGICAL_AND",
                                          "OPERATOR_BITWISE_OR",
                                          "OPERATOR_BITWISE_XOR",
                                          "OPERATOR_BITWISE_AND",

                                          "OPERATOR_COMP_EQ",
                                          "OPERATOR_COMP_NEQ",

                                          "OPERATOR_COMP_LESS",
                                          "OPERATOR_COMP_MORE",
                                          "OPERATOR_COMP_LESSEQ",
                                          "OPERATOR_COMP_MOREEQ",

                                          "OPERATOR_BITWISE_SHL",
                                          "OPERATOR_BITWISE_SHR",
                                          "OPERATOR_LOGICAL_NOT",
                                          "OPERATOR_BITWISE_NOT",

                                          "LEFT_PAREN",
                                          "RIGHT_PAREN",
                                          "LEFT_BRACKET",
                                          "RIGHT_BRACKET",
                                          "LEFT_BRACE",
                                          "RIGHT_BRACE",
                                          "SEMICOLON",
                                          "COLON",
                                          "DOT",
                                          "COMMA"};

    return (int)type > (int)Token::Type::DOT ? "UNKNOWN" : map[(int)type];
}

std::string operator_to_string(Token::Type type) {
    switch (type) {
    case Token::Type::OPERATOR_PLUS:
        return "+";
    case Token::Type::OPERATOR_ASSIGNMENT:
        return "=";
    case Token::Type::OPERATOR_MINUS:
        return "-";
    case Token::Type::OPERATOR_PERCENT:
        return "%";
    case Token::Type::OPERATOR_SLASH:
        return "/";
    case Token::Type::OPERATOR_STAR:
        return "*";

    case Token::Type::OPERATOR_LOGICAL_OR:
        return "||";
    case Token::Type::OPERATOR_LOGICAL_AND:
        return "&&";
    case Token::Type::OPERATOR_BITWISE_OR:
        return "|";
    case Token::Type::OPERATOR_BITWISE_AND:
        return "&";
    case Token::Type::OPERATOR_BITWISE_XOR:
        return "^";
    case Token::Type::OPERATOR_COMP_EQ:
        return "==";
    case Token::Type::OPERATOR_COMP_NEQ:
        return "!=";

    case Token::Type::OPERATOR_COMP_LESS:
        return "<";
    case Token::Type::OPERATOR_COMP_MORE:
        return ">";
    case Token::Type::OPERATOR_COMP_LESSEQ:
        return "<=";
    case Token::Type::OPERATOR_COMP_MOREEQ:
        return ">=";
    case Token::Type::OPERATOR_BITWISE_SHL:
        return "<<";
    case Token::Type::OPERATOR_BITWISE_SHR:
        return ">>";

    case Token::Type::OPERATOR_BITWISE_NOT:
        return "~";
    case Token::Type::OPERATOR_LOGICAL_NOT:
        return "!";
    default:
        return "??? idk vro";
    }
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "{ type: " << token_type_to_string(token.type) << ", value: '" << token.value << "' }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Token>& tokens) {
    for (Token token : tokens) {
        os << token << '\n';
    }

    return os;
}
} // namespace yuzu