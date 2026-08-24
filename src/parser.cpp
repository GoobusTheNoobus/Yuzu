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

#include "parser.hpp"
#include "ast.hpp"
#include "token.hpp"
#include "error.hpp"
#include <cassert>
#include <charconv>
#include <cstdint>
#include <system_error>
#include <vector>

namespace yuzu 
{

    SyntaxTree Parser::parse()
    {
        SyntaxTree tree;
        tree_ = &tree;

        auto root = tree.make<SyntaxTree::Root>();

        while (!end() && peek().type != Token::Type::END_OF_FILE)
        {
            if (match(Token::Type::SEMICOLON)) continue;

            root->children.push_back( parse_statement());
        }

        tree_ = nullptr; // to prevent memory stuff

        return tree;
    }

    SyntaxTree::Node* Parser::parse_statement()
    {
        Token current = peek();

        switch (current.type)
        {
            case Token::Type::KEYWORD_LET:
            {
                next();
                if (!check(Token::Type::IDENTIFIER))
                
                    raise_error("Expected identifier, got '" + peek().value + "'");
                

                std::string name = peek().value;
                std::string type;

                if (match(Token::Type::COLON))
                {
                    if (!check(Token::Type::IDENTIFIER))
                        raise_error("Expected type name, got '" + peek().value + "'");

                    type = next().value;
                } else {
                    next();
                }

                SyntaxTree::Node* value = nullptr;
                if (match(Token::Type::OPERATOR_ASSIGNMENT))
                {
                    value = parse_expression();
                }
                else if (type.empty()) 
                {
                    std::cout << token_type_to_string(peek().type) << std::endl;
                    raise_error("Cannot deduce type from declataion");
                }
                    

                // declaration & saving

                auto id = tree_->make<SyntaxTree::Id>(name);
                auto ass = tree_->make<SyntaxTree::OpBinary>(id, value, Token::Type::OPERATOR_ASSIGNMENT);
                auto vardecl = tree_->make<SyntaxTree::StmVarDecl>(ass, type);

                if (!match(Token::Type::SEMICOLON))
                    raise_error("Expected ';', got '" + peek().value + "'");

                return vardecl;
            }

            case Token::Type::KEYWORD_EXIT:
            {
                next();

                auto status = parse_expression();
                auto stm = tree_->make<SyntaxTree::StmExit>(status);

                if (!match(Token::Type::SEMICOLON))
                    raise_error("Expected ';', got '" + peek().value + "'");

                return stm;
            }

            case Token::Type::KEYWORD_RETURN:
            {
                next();

                auto value = parse_expression();
                auto stm = tree_->make<SyntaxTree::StmReturn>(value);

                if (!match(Token::Type::SEMICOLON))
                    raise_error("Expected ';', got '" + peek().value + "'");

                return stm;
            }

            case Token::Type::KEYWORD_FUNC:
            {
                next(); // consume 'func'

                if (!check(Token::Type::IDENTIFIER))
                    raise_error("Expected identifier, got '" + peek().value + "'");

                std::string name = peek().value;
                next();

                if (!match(Token::Type::LEFT_PAREN))
                    raise_error("Expected '(', got '" + peek().value + "'");

                std::vector<SyntaxTree::FuncParam> params;

                if (!check(Token::Type::RIGHT_PAREN))
                {
                    while (true)
                    {
                        // parameter name
                        if (!check(Token::Type::IDENTIFIER))
                            raise_error(
                                "Expected parameter name, got '" + peek().value + "'");

                        std::string param_name = peek().value;
                        next();

                        if (!match(Token::Type::COLON))
                            raise_error(
                                "Expected ':' after parameter name, got '" + peek().value + "'");

                        // parameter type
                        if (!check(Token::Type::IDENTIFIER))
                            raise_error(
                                "Expected parameter type, got '" + peek().value + "'");

                        std::string param_type = peek().value;
                        next();

                        params.push_back({param_name, param_type});

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

                return tree_->make<SyntaxTree::StmFunc>(block, name, return_type, params);
            }

            default:
            {
                auto expr = parse_expression();
                if (!match(Token::Type::SEMICOLON))
                    raise_error("Expected ';', got '" + peek().value + "'");
                return expr;
            }
        }
    }

    SyntaxTree::Node* Parser::parse_expression()
    {
        return parse_assignment();
    }

    SyntaxTree::Block* Parser::parse_block()
    {
        auto block = tree_->make<SyntaxTree::Block>();

        while (!end() && !(check(Token::Type::END_OF_FILE) || check(Token::Type::RIGHT_BRACE)))
        {
            block->children.push_back( parse_statement());

        }

        return block;
    }

    SyntaxTree::Node* Parser::parse_primary()
    {
        auto token = peek();
        next();

        SyntaxTree::Node* node;
        switch (token.type)
        {
            case (Token::Type::LITERAL_INT):
            {
                int64_t value;

                auto [ptr, ec] = std::from_chars(token.value.data(), token.value.data() + token.value.size(), value);

                if (ec != std::errc())
                {
                    // invalid number
                    raise_error("Integer literal '" + token.value + "' out of range");
                }

                node = tree_->make<SyntaxTree::LitInt>(value);
                break;
            }
            case (Token::Type::LITERAL_FLOAT):
            {
                double value;

                auto [ptr, ec] = std::from_chars(token.value.data(), token.value.data() + token.value.size(), value);

                if (ec == std::errc::invalid_argument)
                {
                    raise_error("Invalid floating-point literal '" + token.value + "'");
                }

                else if (ec == std::errc::result_out_of_range)
                {
                    raise_error("Floating-point literal '" + token.value + "' out of range");
                }

                node = tree_->make<SyntaxTree::LitFloat>(value);
                break;
            }

            case (Token::Type::LITERAL_CHAR):
            {
                char value = token.value[0];
                assert(token.value.size() == 1);

                node = tree_->make<SyntaxTree::LitChar>(value);
                break;
            }

            case (Token::Type::LITERAL_STRING):
            {
                node = tree_->make<SyntaxTree::LitString>(token.value);
                break;
            }

            case (Token::Type::KEYWORD_TRUE):
            {
                node = tree_->make<SyntaxTree::LitBool>(true);
                break;
            }

            case (Token::Type::KEYWORD_FALSE):
            {
                node = tree_->make<SyntaxTree::LitBool>(false);
                break;
            }

            case (Token::Type::IDENTIFIER):
            {
                node = tree_->make<SyntaxTree::Id>(token.value);
                break;
            }

            case (Token::Type::LEFT_PAREN):
            {
                node = parse_expression();
                if (!match(Token::Type::RIGHT_PAREN))
                    raise_error("Expected ')', got '" + peek().value + "'");
                break;
            }

            default:
            {
                raise_error("Unknown token '" + token.value + "'");
            }
        }

        return node;
    }

    SyntaxTree::Node* Parser::parse_assignment()
    {
        auto left = parse_additive();

        if (match(Token::Type::OPERATOR_ASSIGNMENT))
        {
            Token::Type op = Token::Type::OPERATOR_ASSIGNMENT;

            auto right = parse_assignment();

            return tree_->make<SyntaxTree::OpBinary>(left, right, op);
        }

        return left;
    }

    SyntaxTree::Node* Parser::parse_additive()
    {
        auto left = parse_multiplicative();

        while (check(Token::Type::OPERATOR_PLUS) || check(Token::Type::OPERATOR_MINUS))
        {
            Token::Type op = next().type;
            auto right = parse_multiplicative();

            left = tree_->make<SyntaxTree::OpBinary>(left, right, op);
        }

        return left;
    }

    SyntaxTree::Node* Parser::parse_multiplicative()
    {
        auto left = parse_unary();

        while (check(Token::Type::OPERATOR_STAR) || check(Token::Type::OPERATOR_SLASH) || 
               check(Token::Type::OPERATOR_PERCENT))
        {
            Token::Type op = next().type;
            auto right = parse_unary();

            left = tree_->make<SyntaxTree::OpBinary>(left, right, op);
        }

        return left;
    }

    SyntaxTree::Node* Parser::parse_unary()
    {
        if (check(Token::Type::OPERATOR_MINUS))
        {
            Token::Type op = next().type;
            auto operand = parse_unary();

            return tree_->make<SyntaxTree::OpUnary>(operand, op);
        }

        return parse_postfix();
    }

    SyntaxTree::Node* Parser::parse_postfix()
    {
        auto* expr = parse_primary();

        while (true)
        {
            if (match(Token::Type::LEFT_PAREN))
            {
                std::vector<SyntaxTree::Node*> args;

                if (!check(Token::Type::RIGHT_PAREN))
                {
                    do {
                        args.push_back(parse_expression());
                    }while (match(Token::Type::COMMA));
                }

                if (!match(Token::Type::RIGHT_PAREN))
                    raise_error("Expected ')', got '" + peek().value + "'");

                expr = tree_->make<SyntaxTree::Call>(expr, args);
                continue;
            }

            break;
        }

        return expr;
    }
}