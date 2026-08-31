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

#include "frontend/lexer/token.hpp"
#include "frontend/parser/ast/declarations.hpp"
#include "frontend/parser/ast/node.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace yuzu {
// turns token list to abstract syntax tree
class Parser {
  public:
    Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}
    std::unique_ptr<Root> parse();

  private:
    const std::vector<Token> tokens_;
    usize pos_ = 0;

    inline const Token& peek(isize offset = 0) const {
        const auto index = std::clamp<isize>(isize(pos_) + offset, 0, tokens_.size() - 1);

        return tokens_.at(index);
    }
    inline const Token& next() {
        ++pos_;
        return peek(-1);
    }
    inline bool end() const { return pos_ >= tokens_.size(); }
    inline bool check(Token::Type t) const { return peek().type == t; }
    inline bool match(Token::Type t) {
        if (end() || peek().type != t) {
            return false;
        }

        next();
        return true;
    }

    std::unique_ptr<BaseNode> parse_statement();
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Block> parse_block();

    std::unique_ptr<Expression> parse_logical_or();
    std::unique_ptr<Expression> parse_logical_and();
    std::unique_ptr<Expression> parse_bitwise_or();
    std::unique_ptr<Expression> parse_bitwise_xor();
    std::unique_ptr<Expression> parse_bitwise_and();

    std::unique_ptr<Expression> parse_eq_comparison();
    std::unique_ptr<Expression> parse_lm_comparison();
    std::unique_ptr<Expression> parse_bitwise_shifts();

    std::unique_ptr<Expression> parse_additive();
    std::unique_ptr<Expression> parse_multiplicative();
    std::unique_ptr<Expression> parse_unary();
    std::unique_ptr<Expression> parse_postfix();
    std::unique_ptr<Expression> parse_primary();

    std::vector<Func::Param> parse_parameters();
};
} // namespace yuzu