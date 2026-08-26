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

namespace yuzu
{
    enum class NativeType : u8
    {
        INT8, INT16, INT32, INT64,
        FLOAT32, FLOAT64, CHAR, BOOL, STRING, VOID
    };

    constexpr bool is_int(NativeType t)
    {
        return t == NativeType::INT8 || t == NativeType::INT16 ||
               t == NativeType::INT32 || t == NativeType::INT64;
    }

    constexpr bool is_float(NativeType t)
    {
        return t == NativeType::FLOAT32 || t == NativeType::FLOAT64;
    }

    constexpr bool is_numeric(NativeType t)
    {
        return t != NativeType::BOOL && t != NativeType::STRING &&
               t != NativeType::VOID;
    }

    constexpr NativeType common(NativeType t1, NativeType t2, Token::Type op)
    {
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
}