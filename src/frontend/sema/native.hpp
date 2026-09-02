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
#include <cstdint>
#include <optional>
#include <string>
namespace yuzu {

inline bool is_integer(const std::string& type) {
    return type == "int8" || type == "int16" || type == "int32" || type == "int64" ||
           type == "char";
}
inline bool is_float(const std::string& type) { return type == "float32" || type == "float64"; }
inline bool is_numeric(const std::string& type) { return is_float(type) || is_integer(type); }

inline bool is_native_type(const std::string& type) {
    return is_numeric(type) || type == "bool" || type == "string";
}

inline usize type_size(const std::string& type) {
    if (type == "int8")
        return 1;
    if (type == "int16")
        return 2;
    if (type == "int32")
        return 4;
    if (type == "int64")
        return 8;
    if (type == "float32")
        return 4;
    if (type == "float64")
        return 8;
    if (type == "char")
        return 1;
    if (type == "bool")
        return 1;
    if (type == "string")
        return 8;
    return 0;
}

inline bool is_implicitly_convertible(const std::string& from_type, const std::string& to_type) {

    if (from_type == to_type)
        return true;

    if (is_integer(from_type) && is_integer(to_type))
        return type_size(from_type) <= type_size(to_type) || to_type == "int32";

    if (is_integer(from_type) && is_float(to_type))
        return true;

    if (is_float(from_type) && is_integer(to_type))
        return true;

    if (is_float(from_type) && is_float(to_type))
        return true;

    return false;
}

inline bool is_explicitly_convertible(const std::string& from_type, const std::string& to_type) {

    if (is_numeric(from_type) && is_numeric(to_type))
        return true;

    return false;
}

inline bool fits_in(i64 lit, const std::string& type) {
    if (!is_numeric(type))
        return false;

    if (type == "int8")
        return lit >= INT8_MIN && lit <= INT8_MAX;

    if (type == "int16")
        return lit >= INT16_MIN && lit <= INT16_MAX;

    if (type == "int32")
        return lit >= INT32_MIN && lit <= INT32_MAX;

    if (type == "int64")
        return lit >= INT64_MIN && lit <= INT64_MAX;

    return false;
}

inline std::optional<std::string> binary_common_type(const std::string& left,
                                                     const std::string& right, Token::Type op) {
    if (is_integer(left) && is_integer(right)) {
        switch (op) {
        case yuzu::Token::Type::OPERATOR_PLUS:
        case yuzu::Token::Type::OPERATOR_MINUS:
        case yuzu::Token::Type::OPERATOR_STAR:
        case yuzu::Token::Type::OPERATOR_SLASH:
        case yuzu::Token::Type::OPERATOR_PERCENT:
        case yuzu::Token::Type::OPERATOR_BITWISE_AND:
        case yuzu::Token::Type::OPERATOR_BITWISE_OR:
        case yuzu::Token::Type::OPERATOR_BITWISE_XOR:
            return type_size(left) >= type_size(right) ? left : right;

        case yuzu::Token::Type::OPERATOR_ASSIGNMENT:
            return left;

        case yuzu::Token::Type::OPERATOR_BITWISE_SHL:
        case yuzu::Token::Type::OPERATOR_BITWISE_SHR:
            return left;

        case yuzu::Token::Type::OPERATOR_COMP_EQ:
        case yuzu::Token::Type::OPERATOR_COMP_NEQ:
        case yuzu::Token::Type::OPERATOR_COMP_LESSEQ:
        case yuzu::Token::Type::OPERATOR_COMP_LESS:
        case yuzu::Token::Type::OPERATOR_COMP_MOREEQ:
        case yuzu::Token::Type::OPERATOR_COMP_MORE:
            return "bool";

        default:
            return {};
        }
    }

    if (is_float(left) && is_integer(right)) {
        switch (op) {
        case yuzu::Token::Type::OPERATOR_PLUS:
        case yuzu::Token::Type::OPERATOR_MINUS:
        case yuzu::Token::Type::OPERATOR_STAR:
        case yuzu::Token::Type::OPERATOR_SLASH:
            return left;

        case yuzu::Token::Type::OPERATOR_ASSIGNMENT:
            return left;

        case yuzu::Token::Type::OPERATOR_COMP_EQ:
        case yuzu::Token::Type::OPERATOR_COMP_NEQ:
        case yuzu::Token::Type::OPERATOR_COMP_LESSEQ:
        case yuzu::Token::Type::OPERATOR_COMP_LESS:
        case yuzu::Token::Type::OPERATOR_COMP_MOREEQ:
        case yuzu::Token::Type::OPERATOR_COMP_MORE:
            return "bool";

        default:
            return {};
        }
    }

    if (is_integer(left) && is_float(right)) {
        switch (op) {
        case yuzu::Token::Type::OPERATOR_PLUS:
        case yuzu::Token::Type::OPERATOR_MINUS:
        case yuzu::Token::Type::OPERATOR_STAR:
        case yuzu::Token::Type::OPERATOR_SLASH:
            return right;

        case yuzu::Token::Type::OPERATOR_ASSIGNMENT:
            return left;

        case yuzu::Token::Type::OPERATOR_COMP_EQ:
        case yuzu::Token::Type::OPERATOR_COMP_NEQ:
        case yuzu::Token::Type::OPERATOR_COMP_LESSEQ:
        case yuzu::Token::Type::OPERATOR_COMP_LESS:
        case yuzu::Token::Type::OPERATOR_COMP_MOREEQ:
        case yuzu::Token::Type::OPERATOR_COMP_MORE:
            return "bool";

        default:
            return {};
        }
    }

    if (is_float(left) && is_float(right)) {
        switch (op) {
        case yuzu::Token::Type::OPERATOR_PLUS:
        case yuzu::Token::Type::OPERATOR_MINUS:
        case yuzu::Token::Type::OPERATOR_STAR:
        case yuzu::Token::Type::OPERATOR_SLASH:
            return type_size(left) >= type_size(right) ? left : right;

        case yuzu::Token::Type::OPERATOR_ASSIGNMENT:
            return left;

        case yuzu::Token::Type::OPERATOR_COMP_EQ:
        case yuzu::Token::Type::OPERATOR_COMP_NEQ:
        case yuzu::Token::Type::OPERATOR_COMP_LESSEQ:
        case yuzu::Token::Type::OPERATOR_COMP_LESS:
        case yuzu::Token::Type::OPERATOR_COMP_MOREEQ:
        case yuzu::Token::Type::OPERATOR_COMP_MORE:
            return "bool";

        default:
            return {};
        }
    }

    if (left == "bool" && right == "bool") {
        switch (op) {
        case yuzu::Token::Type::OPERATOR_COMP_EQ:
        case yuzu::Token::Type::OPERATOR_COMP_NEQ:
        case yuzu::Token::Type::OPERATOR_LOGICAL_AND:
        case yuzu::Token::Type::OPERATOR_LOGICAL_OR:
            return "bool";

        default:
            return {};
        }
    }

    if (left == "string" && op == Token::Type::OPERATOR_PLUS) {
        return left;
    }

    return {};
}

inline std::optional<std::string> unary_type(const std::string& operand, Token::Type op) {
    switch (op) {
    case yuzu::Token::Type::OPERATOR_PLUS:
        return is_numeric(operand) ? std::optional<std::string>{operand} : std::nullopt;
    case yuzu::Token::Type::OPERATOR_MINUS:
        return is_numeric(operand) ? std::optional<std::string>{operand} : std::nullopt;
    case yuzu::Token::Type::OPERATOR_BITWISE_NOT:
        return is_integer(operand) ? std::optional<std::string>{operand} : std::nullopt;
    case yuzu::Token::Type::OPERATOR_LOGICAL_NOT:
        return operand == "bool" ? std::optional<std::string>{operand} : std::nullopt;
    default:
        return {};
    }
}
} // namespace yuzu