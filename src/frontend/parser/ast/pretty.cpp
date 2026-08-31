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

#include "frontend/parser/ast/pretty.hpp"
#include "core/type.hpp"
#include "frontend/lexer/token.hpp"
#include "frontend/parser/ast/declarations.hpp"
#include "frontend/parser/ast/expressions.hpp"
#include "frontend/parser/ast/node.hpp"
#include "frontend/parser/ast/statements.hpp"
#include <iostream>

namespace yuzu {

static std::string get_indent(int indent) { return std::string(indent, ' '); }

void pretty_print(const Root& root) {
    std::cout << "{\n";

    for (const auto& node : root.children) {
        pretty_print(*node, 2);
    }

    std::cout << "}\n";
}

void pretty_print(const BaseNode& node, int indent) {
    switch (node.kind) {
    case NodeKind::LITERAL_INT: {
        std::cout << get_indent(indent) << static_cast<const IntLiteral&>(node).value << '\n';
        return;
    }

    case NodeKind::LITERAL_FLOAT: {
        std::cout << get_indent(indent) << static_cast<const FloatLiteral&>(node).value << '\n';
        return;
    }

    case NodeKind::LITERAL_CHAR: {
        std::cout << get_indent(indent) << '\'' << static_cast<const CharLiteral&>(node).value
                  << '\'' << '\n';
        return;
    }

    case NodeKind::LITERAL_BOOL: {
        std::cout << get_indent(indent)
                  << (static_cast<const BoolLiteral&>(node).value ? "true" : "false") << '\n';
        return;
    }

    case NodeKind::LITERAL_STRING: {
        std::cout << get_indent(indent) << '"' << static_cast<const StringLiteral&>(node).value
                  << '"' << '\n';
        return;
    }

    case NodeKind::IDENTIFIER: {
        std::cout << get_indent(indent) << static_cast<const Identifier&>(node).name << '\n';
        return;
    }

    case NodeKind::RETURN: {
        std::cout << get_indent(indent) << "return " << '\n';
        pretty_print(*static_cast<const Return&>(node).value, indent + 2);
        return;
    }

    case NodeKind::EXIT: {
        std::cout << get_indent(indent) << "exit " << '\n';
        pretty_print(*static_cast<const Exit&>(node).value, indent + 2);
        return;
    }

    case NodeKind::LET: {
        auto& let = static_cast<const Let&>(node);
        std::cout << get_indent(indent) << "let " << let.name << ':' << native_to_string(let.type);

        if (let.value) {
            std::cout << " = \n";
            pretty_print(**let.value, indent + 2);
        }

        return;
    }

    case NodeKind::OP_BINARY: {
        auto& binary = static_cast<const BinaryOp&>(node);

        std::cout << get_indent(indent) << "operation\n";

        pretty_print(*binary.left, indent + 2);

        std::cout << get_indent(indent + 2) << operator_to_string(binary.op) << '\n';

        pretty_print(*binary.right, indent + 2);

        return;
    }

    case NodeKind::OP_UNARY: {
        auto& unary = static_cast<const UnaryOp&>(node);

        std::cout << get_indent(indent) << "operation" << operator_to_string(unary.op) << '\n';

        pretty_print(*unary.operand, indent + 2);

        return;
    }

    case NodeKind::FUNC: {
        auto& func = static_cast<const Func&>(node);

        std::cout << get_indent(indent) << "func " << func.name << "(";

        for (int i = 0; i < func.params.size(); ++i) {
            auto& param = func.params[i];
            std::cout << param.name << ": " << native_to_string(param.type);

            if (i != func.params.size() - 1)
                std::cout << ", ";
        }

        std::cout << ") : " << native_to_string(func.type) << '\n';
        pretty_print(*func.body, indent + 2);

        return;
    }

    case NodeKind::BLOCK: {
        std::cout << get_indent(indent) << "{\n";
        for (const auto& node : static_cast<const Block&>(node).children) {
            pretty_print(*node, 2);
        }
        std::cout << get_indent(indent) << "}\n";

        return;
    }

    default:
        std::cout << get_indent(indent) << "idk\n";
    }
}

} // namespace yuzu