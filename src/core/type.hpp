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
#include "core/error.hpp"
#include "frontend/lexer/token.hpp"
#include <cassert>
#include <cstdint>

namespace yuzu {

enum class NativeType : u8 {
    INT8,
    INT16,
    INT32,
    INT64,
    FLOAT32,
    FLOAT64,
    CHAR,
    BOOL,
    STRING,
    VOID,
    AUTO
};

inline std::string native_to_string(NativeType t) {
    switch (t) {
    case yuzu::NativeType::BOOL:
        return "bool";
    case yuzu::NativeType::CHAR:
        return "char";
    case yuzu::NativeType::FLOAT32:
        return "float32";
    case yuzu::NativeType::FLOAT64:
        return "float64";
    case yuzu::NativeType::INT8:
        return "int8";
    case yuzu::NativeType::INT16:
        return "int16";
    case yuzu::NativeType::INT32:
        return "int32";
    case yuzu::NativeType::INT64:
        return "int64";
    case yuzu::NativeType::STRING:
        return "string";
    case yuzu::NativeType::VOID:
        return "void";
    case yuzu::NativeType::AUTO:
        return "auto";
    default:
        return "errortype";
    }
}

inline NativeType string_to_native(const std::string& str) {
    if (str == "bool")
        return NativeType::BOOL;
    if (str == "char")
        return NativeType::CHAR;
    if (str == "float32")
        return NativeType::FLOAT32;
    if (str == "float64")
        return NativeType::FLOAT64;
    if (str == "int8")
        return NativeType::INT8;
    if (str == "int16")
        return NativeType::INT16;
    if (str == "int32")
        return NativeType::INT32;
    if (str == "int64")
        return NativeType::INT64;
    if (str == "string")
        return NativeType::STRING;
    if (str == "void")
        return NativeType::VOID;
    return NativeType::VOID;
}

constexpr bool is_int(NativeType t) {
    return t == NativeType::INT8 || t == NativeType::INT16 || t == NativeType::INT32 ||
           t == NativeType::INT64;
}

constexpr bool is_float(NativeType t) {
    return t == NativeType::FLOAT32 || t == NativeType::FLOAT64;
}

constexpr bool is_numeric(NativeType t) {
    return t != NativeType::BOOL && t != NativeType::STRING && t != NativeType::VOID;
}

constexpr NativeType common(NativeType t1, NativeType t2, Token::Type op) {
    if (is_int(t1) && is_int(t2))
        return (NativeType)(std::max(t1, t2));

    if (is_int(t1) && is_float(t2))
        return t2;

    if (is_float(t1) && is_int(t2))
        return t1;

    if (is_float(t1) && is_float(t2))
        return (NativeType)(std::max(t1, t2));

    if (t1 == NativeType::CHAR && is_numeric(t2))
        return t2;

    if (t2 == NativeType::CHAR && is_numeric(t1))
        return t1;

    if (t1 == NativeType::STRING && op == Token::Type::OPERATOR_PLUS)
        return t1;

    if (t2 == NativeType::STRING && op == Token::Type::OPERATOR_PLUS)
        return t2;

    raise_error("Unknown type combinations");
    return NativeType::VOID;
}

constexpr bool fits_in(i64 lit, NativeType type) {
    assert(is_int(type));

    switch (type) {
    case NativeType::INT8:
        return lit >= INT8_MIN && lit <= INT8_MAX;
    case NativeType::INT16:
        return lit >= INT16_MIN && lit <= INT16_MAX;
    case NativeType::INT32:
        return lit >= INT32_MIN && lit <= INT32_MAX;
    case NativeType::INT64:
        return lit >= INT64_MIN && lit <= INT64_MAX;
    default:
        return 67;
    }
}

constexpr bool is_implicitly_convertible(NativeType from, NativeType to) {
    if (is_int(from) && is_int(to))
        return to >= from;

    if (is_int(from) && is_float(to))
        return true;

    if (is_float(from) && is_int(to))
        return true;

    if (is_float(to) && is_float(from))
        return true;

    return false;
}
} // namespace yuzu