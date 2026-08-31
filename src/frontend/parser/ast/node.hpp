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
#include <memory>
#include <vector>

namespace yuzu {

enum class NodeKind : u8 {
    ROOT,
    BLOCK,

    IDENTIFIER,
    CAST,
    CALL,

    LITERAL_INT,
    LITERAL_FLOAT,
    LITERAL_STRING,
    LITERAL_CHAR,
    LITERAL_BOOL,

    EXIT,
    RETURN,

    LET,
    FUNC,

    OP_BINARY,
    OP_UNARY,
};

struct BaseNode {
    NodeKind kind;

    BaseNode(NodeKind kind) : kind(kind) {}
    virtual ~BaseNode() = default;
};

struct Expression : public BaseNode {
    Expression(NodeKind kind) : BaseNode(kind) {}
};

struct Declaration : public BaseNode {
    Declaration(NodeKind kind) : BaseNode(kind) {}
};

struct Statement : public BaseNode {
    Statement(NodeKind kind) : BaseNode(kind) {}
};

struct Root : public BaseNode {
    std::vector<std::unique_ptr<BaseNode>> children;
    Root() : BaseNode(NodeKind::ROOT) {}
};

struct Block : public BaseNode {
    std::vector<std::unique_ptr<BaseNode>> children;
    Block() : BaseNode(NodeKind::BLOCK) {}
};

} // namespace yuzu