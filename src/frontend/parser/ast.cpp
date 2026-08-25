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

#include "frontend/parser/ast.hpp"
#include "frontend/lexer/token.hpp"
#include <cassert>
#include <cfloat>
#include <iostream>

namespace yuzu
{

    void SyntaxTree::free_all()
    {
        for (Node* node : nodes)
        {
            delete node;
        }
    }

    namespace 
    {
        std::string get_indent(int indent) { return std::string(indent, ' '); }
    }

    void SyntaxTree::pretty_print()
    {
        if (nodes.size() < 1)
        {
            std::cout << "[Empty AST]";
            return;
        }
            

        assert(nodes.front()->type == SyntaxTree::NodeType::ROOT);

        std::cout << "Root\n";

        for (const Node* node : static_cast<Root*>(nodes.front())->children)
        {
            pretty_print(node, 2);
        }
    }

    void SyntaxTree::pretty_print(const Node* node, int indent)
    {
        switch (node->type)
        {
            case NodeType::LITERAL_INT:
            {
                std::cout << get_indent(indent) << "IntLiteral " << static_cast<const LitInt*>(node)->lit
                          << std::endl;
                return;
            }
            case NodeType::LITERAL_FLOAT:
            {
                std::cout << get_indent(indent) << "FloatLiteral " << static_cast<const LitFloat*>(node)->lit
                          << std::endl;
                return;
            }
            case NodeType::LITERAL_CHAR:
            {
                std::cout << get_indent(indent) << "CharLiteral '" << static_cast<const LitChar*>(node)->lit
                          << '\'' << std::endl;
                return;
            }
            case NodeType::LITERAL_STRING:
            {
                std::cout << get_indent(indent) << "StringLiteral \"" << static_cast<const LitString*>(node)->lit
                          << '"' << std::endl;
                return;
            }
            case NodeType::LITERAL_BOOL:
            {
                std::cout << get_indent(indent) << "BoolLiteral " << (static_cast<const LitBool*>(node)->lit ? 
                            "true" : "false"
                          ) << std::endl;
                return;
            }
            case NodeType::IDENTIFIER:
            {
                std::cout << get_indent(indent) << "Identifier " << static_cast<const Id*>(node)->name << std::endl;
                return;
            }
            case NodeType::STATEMENT_RETURN:
            {
                std::cout << get_indent(indent) << "Return \n";
                pretty_print(static_cast<const StmReturn*>(node)->value, indent + 2);
                return;
            }
            case NodeType::STATEMENT_EXIT:
            {
                std::cout << get_indent(indent) << "Exit \n";
                pretty_print(static_cast<const StmExit*>(node)->status, indent + 2);
                return;
            }
            case NodeType::STATEMENT_VARDECL:
            {
                std::cout << get_indent(indent) << "Let \n";
                pretty_print(static_cast<const StmVarDecl*>(node)->ass, indent + 2);
                return;
            }
            case NodeType::OPERATION_BINARY:
            {
                auto operation = static_cast<const OpBinary*>(node);
                std::cout << get_indent(indent) << "Operation (" << std::endl;

                pretty_print(operation->left, indent + 2);
                std::cout << get_indent(indent + 2) << operator_to_string(operation->op) << std::endl;

                // account for uninitialized variable
                if (operation->right)
                    pretty_print(operation->right, indent + 2);
                else
                    std::cout << get_indent(indent + 2) << "None";

                std::cout << get_indent(indent) << ")" << std::endl;

                return;
            }
            case NodeType::OPERATION_UNARY:
            {
                auto operation = static_cast<const OpUnary*>(node);
                std::cout << get_indent(indent) << "Operation (" << operator_to_string(operation->op) << std::endl;

                pretty_print(operation->operand, indent + 2);

                std::cout << get_indent(indent) << ")\n";

                return;
            }
            case NodeType::BLOCK:
            {
                std::cout << get_indent(indent) << "Block {" << std::endl;

                for (auto stm : static_cast<const Block*>(node)->children)
                {
                    pretty_print(stm, indent + 2);
                }

                std::cout << get_indent(indent) << "}" << std::endl;
                return;
            }
            case NodeType::STATEMENT_FUNC:
            {
                auto func = static_cast<const StmFunc*>(node);
                std::cout << get_indent(indent) << "Func " << func->name << std::endl;
                std::cout << get_indent(indent + 2) << "Args {";

                for (auto param : func->args)
                {
                    std::cout << param.type << " " << param.name << " ";
                }
                std::cout << "}" << std::endl;

                std::cout << get_indent(indent + 2) << "returns " << func->type << std::endl;

                pretty_print(func->block, indent + 2);
                return;
            }
            case NodeType::CALL:
            {
                auto call = static_cast<const Call*>(node);

                std::cout << get_indent(indent) << "Call ";
                pretty_print(call->callee, 0);
                std::cout << std::endl;
                std::cout << get_indent(indent) << "  Args {" << std::endl;
                
                for (auto arg : call->args)
                {
                    pretty_print(arg, indent + 4);
                }
                std::cout << get_indent(indent) << "  }\n";
                return;
            }
            default:
            {
                std::cout << get_indent(indent) << "Unprintable" << std::endl;
            }
        }
    }
}