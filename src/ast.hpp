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

#include "token.hpp"
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>
namespace yuzu
{
    // represents the entire ast, not just a node
    // APPROACH:
    // since I'm not the kind of guy to use unique_ptr, the syntax tree
    // will have a list of raw pointers to every node created. then, to free
    // we just iterate the list and delete one by one

    class SyntaxTree
    {
        public:
        enum class NodeType : uint8_t
        {
            ROOT, // represents the root node of the AST
            BLOCK,
            IDENTIFIER,

            LITERAL_INT,
            LITERAL_FLOAT,
            LITERAL_STRING,
            LITERAL_CHAR,
            LITERAL_BOOL,

            STATEMENT_EXIT,
            STATEMENT_RETURN,
            STATEMENT_VARDECL, // assignments are just binaryops 
            STATEMENT_FUNC,

            OPERATION_BINARY,
            OPERATION_UNARY,

            CAST,
            CALL,
        };

        // abstract type
        struct Node { NodeType type; Node(NodeType t) : type(t) {} virtual ~Node() = default; };
        
        struct Root  : Node { std::vector<Node*> children; Root() : Node(NodeType::ROOT) {} };
        struct Block : Node { std::vector<Node*> children; Block() : Node(NodeType::BLOCK) {} };

        struct Id : Node { std::string name; Id(const std::string& name) : Node(NodeType::IDENTIFIER), name(name) {} };

        struct LitInt       : Node { int64_t lit; LitInt(int64_t lit) : Node(NodeType::LITERAL_INT), lit(lit) {} };
        struct LitFloat     : Node { double lit; LitFloat(double lit) : Node(NodeType::LITERAL_FLOAT), lit(lit) {} };
        struct LitString    : Node { std::string lit; LitString(std::string lit) : Node(NodeType::LITERAL_STRING), lit(lit) {} };
        struct LitChar      : Node { char lit; LitChar(char lit) : Node(NodeType::LITERAL_CHAR), lit(lit) {} };
        struct LitBool      : Node { bool lit; LitBool(bool lit) : Node(NodeType::LITERAL_BOOL), lit(lit) {} };

        struct OpBinary : Node 
        { 
            Node* left;
            Node* right; 
            Token::Type op;
            OpBinary(Node* left, Node* right, Token::Type op) 
                : Node(NodeType::OPERATION_BINARY), left(left), right(right), op(op) {} 
        };

        struct OpUnary : Node
        {
            Node* operand;
            Token::Type op;
            OpUnary(Node* operand, Token::Type op) 
                : Node(NodeType::OPERATION_UNARY), operand(operand), op(op) {}
        };

        struct Call : Node { Node* callee; std::vector<Node*> args; Call(Node* callee, const std::vector<Node*>& args) 
            : Node(NodeType::CALL), callee(callee), args(args) {} };

        // do not question my formatting 

        struct FuncParam { std::string name, type; };

        struct StmExit    : Node { Node* status; StmExit(Node* status) : Node(NodeType::STATEMENT_EXIT), status(status) {} };
        struct StmReturn  : Node { Node* value; StmReturn(Node* value) : Node(NodeType::STATEMENT_RETURN), value(value) {} };
        struct StmFunc    : Node 
        { 
            std::vector<FuncParam> args; std::string name; std::string type; Block* block;
            StmFunc(Block* block, std::string& name, std::string& type, std::vector<FuncParam> args) 
                    : Node(NodeType::STATEMENT_FUNC), block(block), name(name), type(type), args(args) {} };;
        struct StmVarDecl : Node 
        { 
            OpBinary* ass; // short for assignment
            std::string type;
            StmVarDecl(OpBinary* ass, std::string& type) : Node(NodeType::STATEMENT_VARDECL), ass(ass), type(type)
            {
                assert(ass->op == Token::Type::OPERATOR_ASSIGNMENT);
            }
        };

        struct Cast : Node 
        { 
            Node* thing; // thing to cast, i couldn't think of a better name. tempting to use chatgpt, but I'm going to not
            std::string type;
            Cast(Node* thing, std::string& type) : Node(NodeType::CAST), thing(thing), type(type) {}
        };

        SyntaxTree() = default;
        ~SyntaxTree() = default;
        void free_all();
        void pretty_print();
        void pretty_print(const Node* node, int indent);

        template <typename T, typename ... Args>
        T* make(Args&& ... args)
        {
            T* node = new T(std::forward<Args>(args)...);
            nodes.push_back(node);

            return node;
        }
        
        std::vector<Node*> nodes;
    };
}