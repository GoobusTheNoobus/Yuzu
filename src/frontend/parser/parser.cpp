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

#include "frontend/parser/parser.hpp"
#include "core/core.hpp"
#include "core/error.hpp"
#include "frontend/lexer/token.hpp"
#include "frontend/parser/ast/declarations.hpp"
#include "frontend/parser/ast/expressions.hpp"
#include "frontend/parser/ast/node.hpp"
#include "frontend/parser/ast/statements.hpp"

#include <cassert>
#include <charconv>
#include <memory>
#include <optional>
#include <system_error>
#include <vector>

namespace yuzu {

std::unique_ptr<Root> Parser::parse() {

    auto root = std::make_unique<Root>();

    while (!end() && peek().type != Token::Type::END_OF_FILE) {
        if (match(Token::Type::SEMICOLON))
            continue;

        root->children.push_back(parse_statement());
    }

    return root;
}

std::unique_ptr<BaseNode> Parser::parse_statement() {
    Token current = peek();

    switch (current.type) {
    case Token::Type::KEYWORD_LET: {
        next();
        if (!check(Token::Type::IDENTIFIER))

            raise_error("Expected identifier, got '" + peek().value + "'");

        std::string name = next().value;
        std::string type;

        std::cout << token_type_to_string(peek().type) << std::endl;
        if (match(Token::Type::COLON)) {
            if (!check(Token::Type::IDENTIFIER))
                raise_error("Expected type name, got '" + peek().value + "'");

            type = next().value;
            std::cout << type << std::endl;
        }

        std::optional<std::unique_ptr<Expression>> value = std::nullopt;
        if (match(Token::Type::OPERATOR_ASSIGNMENT)) {
            value = std::move(parse_expression());
        } else if (type.empty()) {
            std::cout << token_type_to_string(peek().type) << std::endl;
            raise_error("Cannot deduce type from declataion");
        }

        auto let = std::make_unique<Let>(name, type, value);

        if (!match(Token::Type::SEMICOLON))
            raise_error("Expected ';', got '" + peek().value + "'");

        return let;
    }

    case Token::Type::KEYWORD_EXIT: {
        next();

        auto stm = std::make_unique<Exit>(parse_expression());

        if (!match(Token::Type::SEMICOLON))
            raise_error("Expected ';', got '" + peek().value + "'");

        return stm;
    }

    case Token::Type::KEYWORD_RETURN: {
        next();

        auto stm = std::make_unique<Return>(parse_expression());

        if (!match(Token::Type::SEMICOLON))
            raise_error("Expected ';', got '" + peek().value + "'");

        return stm;
    }

    case Token::Type::KEYWORD_FUNC: {
        next(); // consume 'func'

        if (!check(Token::Type::IDENTIFIER))
            raise_error("Expected identifier, got '" + peek().value + "'");

        std::string name = peek().value;
        next();

        if (!match(Token::Type::LEFT_PAREN))
            raise_error("Expected '(', got '" + peek().value + "'");

        std::vector<Func::Param> params;

        if (!check(Token::Type::RIGHT_PAREN)) {
            while (true) {
                // parameter name
                if (!check(Token::Type::IDENTIFIER))
                    raise_error("Expected parameter name, got '" + peek().value + "'");

                std::string param_name = peek().value;
                next();

                if (!match(Token::Type::COLON))
                    raise_error("Expected ':' after parameter name, got '" + peek().value + "'");

                // parameter type
                if (!check(Token::Type::IDENTIFIER))
                    raise_error("Expected parameter type, got '" + peek().value + "'");

                std::string param_type = peek().value;
                next();

                params.push_back(Func::Param{param_type, param_name});

                // check if its another parameter
                if (!match(Token::Type::COMMA))
                    break;
            }
        }

        if (!match(Token::Type::RIGHT_PAREN))
            raise_error("Expected ')', got '" + peek().value + "'");

        // return type is required
        if (!match(Token::Type::COLON))
            raise_error("Expected ':' for return type, got '" + peek().value + "'");

        if (!check(Token::Type::IDENTIFIER))
            raise_error("Expected type name, got '" + peek().value + "'");

        std::string return_type = peek().value;
        next();

        if (!match(Token::Type::LEFT_BRACE))
            raise_error("Expected '{', got '" + peek().value + "'");

        auto block = parse_block();

        if (!match(Token::Type::RIGHT_BRACE))
            raise_error("Expected '}', got '" + peek().value + "'");

        return std::make_unique<Func>(name, return_type, params, block);
    }

    default: {
        auto expr = parse_expression();
        if (!match(Token::Type::SEMICOLON))
            raise_error("Expected ';', got '" + peek().value + "'");
        return expr;
    }
    }
}

std::unique_ptr<Expression> Parser::parse_expression() { return parse_logical_or(); }

std::unique_ptr<Block> Parser::parse_block() {
    auto block = std::make_unique<Block>();

    while (!end() && !(check(Token::Type::END_OF_FILE) || check(Token::Type::RIGHT_BRACE))) {
        block->children.push_back(parse_statement());
    }

    return block;
}

std::unique_ptr<Expression> Parser::parse_primary() {
    auto token = peek();
    next();

    switch (token.type) {
    case (Token::Type::LITERAL_INT): {
        i64 value;

        auto [ptr, ec] =
            std::from_chars(token.value.data(), token.value.data() + token.value.size(), value);

        if (ec != std::errc()) {
            // invalid number
            raise_error("Integer literal '" + token.value + "' out of range");
        }

        return std::make_unique<IntLiteral>(value);
    }
    case (Token::Type::LITERAL_FLOAT): {
        double value;

        auto [ptr, ec] =
            std::from_chars(token.value.data(), token.value.data() + token.value.size(), value);

        if (ec == std::errc::invalid_argument) {
            raise_error("Invalid floating-point literal '" + token.value + "'");
        }

        else if (ec == std::errc::result_out_of_range) {
            raise_error("Floating-point literal '" + token.value + "' out of range");
        }

        return std::make_unique<FloatLiteral>(value);
    }

    case (Token::Type::LITERAL_CHAR): {
        char value = token.value[0];
        assert(token.value.size() == 1);

        return std::make_unique<CharLiteral>(value);
    }

    case (Token::Type::LITERAL_STRING): {
        return std::make_unique<StringLiteral>(token.value);
    }

    case (Token::Type::KEYWORD_TRUE): {
        return std::make_unique<BoolLiteral>(true);
    }

    case (Token::Type::KEYWORD_FALSE): {
        return std::make_unique<BoolLiteral>(false);
    }

    case (Token::Type::IDENTIFIER): {
        return std::make_unique<Identifier>(token.value);
    }

    case (Token::Type::LEFT_PAREN): {
        auto node = parse_expression();
        if (!match(Token::Type::RIGHT_PAREN))
            raise_error("Expected ')', got '" + peek().value + "'");
        return node;
    }

    default: {
        raise_error("Expected expression, got '" + token.value + "'");
    }
    }

    return nullptr;
}

std::unique_ptr<Expression> Parser::parse_logical_or() {
    auto left = parse_logical_and();

    while (match(Token::Type::OPERATOR_LOGICAL_OR)) {
        Token::Type op = Token::Type::OPERATOR_LOGICAL_OR;

        auto right = parse_logical_and();

        left = std::make_unique<BinaryOp>(left, right, op);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_logical_and() {
    auto left = parse_bitwise_or();

    while (match(Token::Type::OPERATOR_LOGICAL_AND)) {
        Token::Type op = Token::Type::OPERATOR_LOGICAL_AND;

        auto right = parse_bitwise_or();

        left = std::make_unique<BinaryOp>(left, right, op);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_bitwise_or() {
    auto left = parse_bitwise_xor();

    while (match(Token::Type::OPERATOR_BITWISE_OR)) {
        Token::Type op = Token::Type::OPERATOR_BITWISE_OR;

        auto right = parse_bitwise_xor();

        left = std::make_unique<BinaryOp>(left, right, op);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_bitwise_xor() {
    auto left = parse_bitwise_and();

    while (match(Token::Type::OPERATOR_BITWISE_XOR)) {
        Token::Type op = Token::Type::OPERATOR_BITWISE_XOR;

        auto right = parse_bitwise_and();

        left = std::make_unique<BinaryOp>(left, right, op);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_bitwise_and() {
    auto left = parse_eq_comparison();

    while (match(Token::Type::OPERATOR_BITWISE_AND)) {
        Token::Type op = Token::Type::OPERATOR_BITWISE_AND;

        auto right = parse_eq_comparison();

        left = std::make_unique<BinaryOp>(left, right, op);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_eq_comparison() {
    auto left = parse_lm_comparison();

    while (check(Token::Type::OPERATOR_COMP_EQ) || check(Token::Type::OPERATOR_COMP_NEQ)) {
        Token::Type op = next().type;

        auto right = parse_lm_comparison();

        left = std::make_unique<BinaryOp>(left, right, op);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_lm_comparison() {
    auto left = parse_bitwise_shifts();

    while (check(Token::Type::OPERATOR_COMP_LESS) || check(Token::Type::OPERATOR_COMP_LESSEQ) ||
           check(Token::Type::OPERATOR_COMP_MORE) || check(Token::Type::OPERATOR_COMP_MOREEQ)) {
        Token::Type op = next().type;

        auto right = parse_bitwise_shifts();

        left = std::make_unique<BinaryOp>(left, right, op);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_bitwise_shifts() {
    auto left = parse_additive();

    while (check(Token::Type::OPERATOR_BITWISE_SHL) || check(Token::Type::OPERATOR_BITWISE_SHR)) {
        Token::Type op = next().type;

        auto right = parse_additive();

        left = std::make_unique<BinaryOp>(left, right, op);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_additive() {
    auto left = parse_multiplicative();

    while (check(Token::Type::OPERATOR_PLUS) || check(Token::Type::OPERATOR_MINUS)) {
        Token::Type op = next().type;
        auto right = parse_multiplicative();

        left = std::make_unique<BinaryOp>(left, right, op);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_multiplicative() {
    auto left = parse_unary();

    while (check(Token::Type::OPERATOR_STAR) || check(Token::Type::OPERATOR_SLASH) ||
           check(Token::Type::OPERATOR_PERCENT)) {
        Token::Type op = next().type;
        auto right = parse_unary();

        left = std::make_unique<BinaryOp>(left, right, op);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_unary() {
    if (check(Token::Type::OPERATOR_MINUS) || check(Token::Type::OPERATOR_PLUS) ||
        check(Token::Type::OPERATOR_BITWISE_NOT) || check(Token::Type::OPERATOR_LOGICAL_NOT)) {
        Token::Type op = next().type;
        auto operand = parse_unary();

        return std::make_unique<UnaryOp>(operand, op);
    }

    return parse_postfix();
}

std::unique_ptr<Expression> Parser::parse_postfix() {
    auto expr = parse_primary();

    while (true) {
        if (match(Token::Type::LEFT_PAREN)) {
            std::vector<std::unique_ptr<Expression>> args;

            if (!check(Token::Type::RIGHT_PAREN)) {
                do {
                    args.push_back(parse_expression());
                } while (match(Token::Type::COMMA));
            }

            if (!match(Token::Type::RIGHT_PAREN))
                raise_error("Expected ')', got '" + peek().value + "'");

            expr = std::make_unique<Call>(expr, args);
            continue;
        }

        break;
    }

    return expr;
}
} // namespace yuzu