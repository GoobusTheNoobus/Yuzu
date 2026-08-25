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

#include "frontend/parser/ast.hpp"
#include "frontend/lexer/token.hpp"
#include <algorithm>
#include <cstddef>
#include <vector>

namespace yuzu 
{
    // turns token list to abstract syntax tree
    class Parser
    {
        public:
        Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}
        SyntaxTree parse();

        private:
        const std::vector<Token> tokens_;
        size_t pos_ = 0;
        SyntaxTree* tree_;

        inline const Token& peek(int offset = 0) const
        {
            const auto index = std::clamp<std::ptrdiff_t>(std::ptrdiff_t(pos_) + offset, 0, tokens_.size() - 1);

            return tokens_.at(index);
        }
        inline const Token& next() { ++pos_; return peek(-1); }
        inline bool end() const { return pos_ >= tokens_.size(); }
        inline bool check(Token::Type t) const { return peek().type == t; }
        inline bool match(Token::Type t) 
        { 
            if (end() || peek().type != t)
            {
                return false;
            }

            next();
            return true;
        }

        SyntaxTree::Node* parse_statement();
        SyntaxTree::Node* parse_expression();
        SyntaxTree::Block* parse_block();

        SyntaxTree::Node *parse_assignment();
        SyntaxTree::Node *parse_additive();
        SyntaxTree::Node *parse_multiplicative();
        SyntaxTree::Node *parse_unary();
        SyntaxTree::Node *parse_postfix();
        SyntaxTree::Node *parse_primary();

        std::vector<SyntaxTree::FuncParam> parse_parameters();
    };
}