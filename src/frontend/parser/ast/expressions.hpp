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

#include "core/type.hpp"
#include "frontend/lexer/token.hpp"
#include "frontend/parser/ast/node.hpp"
#include <memory>
#include <vector>

namespace yuzu {

struct IntLiteral : public Expression {
    i64 value;

    IntLiteral(i64 value) : Expression(NodeKind::LITERAL_INT), value(value) {}
};

struct FloatLiteral : public Expression {
    f64 value;

    FloatLiteral(f64 value) : Expression(NodeKind::LITERAL_FLOAT), value(value) {}
};

struct CharLiteral : public Expression {
    char value;

    CharLiteral(char value) : Expression(NodeKind::LITERAL_CHAR), value(value) {}
};

struct BoolLiteral : public Expression {
    bool value;

    BoolLiteral(bool value) : Expression(NodeKind::LITERAL_BOOL), value(value) {}
};

struct StringLiteral : public Expression {
    std::string value;

    StringLiteral(const std::string& value) : Expression(NodeKind::LITERAL_STRING), value(value) {}
};

struct Identifier : public Expression {
    std::string name;

    Identifier(const std::string& name) : Expression(NodeKind::IDENTIFIER), name(name) {}
};

struct Cast : public Expression {
    std::unique_ptr<Expression> expression;
    NativeType type;

    Cast(std::unique_ptr<Expression> expression, NativeType type)
        : Expression(NodeKind::CAST), type(type), expression(std::move(expression)) {}
};

struct Call : public Expression {
    std::unique_ptr<Expression> name;
    std::vector<std::unique_ptr<Expression>> arguments;

    Call(std::unique_ptr<Expression>& name, std::vector<std::unique_ptr<Expression>>& arguments)
        : Expression(NodeKind::CALL), name(std::move(name)), arguments(std::move(arguments)) {}
};

struct BinaryOp : public Expression {
    std::unique_ptr<Expression> left, right;
    Token::Type op;

    BinaryOp(std::unique_ptr<Expression>& left, std::unique_ptr<Expression>& right, Token::Type op)
        : Expression(NodeKind::OP_BINARY), left(std::move(left)), right(std::move(right)), op(op) {}
};

struct UnaryOp : public Expression {
    std::unique_ptr<Expression> operand;
    Token::Type op;

    UnaryOp(std::unique_ptr<Expression>& operand, Token::Type op)
        : Expression(NodeKind::OP_BINARY), operand(std::move(operand)), op(op) {}
};

} // namespace yuzu