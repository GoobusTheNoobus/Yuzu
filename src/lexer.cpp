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

#include "lexer.hpp"
#include "token.hpp"
#include "error.hpp"
#include <cctype>
#include <cstdlib>
#include <string>

namespace yuzu
{

    // copies the file content into the internal string state
    Lexer::Lexer(const std::string& str)
    {
        source_ = str;
        pos_ = 0;
    }

    std::vector<Token> Lexer::tokenize()
    {
        std::vector<Token> tokens;
        p_tokens_ = &tokens;

        while (!end())
        {
            char c = current();

            // skip all space characters, like space, types, and new lines
            if (std::isspace(c)) 
            {
                next();
                continue;
            }

            // comments
            if (c == '#')
            {
                skip_comment();
                continue;
            }

            // identifier/keyword
            if (std::isalpha(c) || c == '_')
            {
                tokenize_word();
                continue;
            }

            // number
            if (std::isdigit(c))
            {
                tokenize_num();
                continue;
            }

            // string
            if (c == '"')
            {
                tokenize_string();
                continue;
            }

            // char
            if (c == '\'')
            {
                tokenize_char();
                continue;
            }

            // symbol
            tokenize_symbol();
        }

        tokens.push_back(Token{"", Token::Type::END_OF_FILE});
        return tokens;
    }

    void Lexer::tokenize_word()
    {
        std::string word(1, current());
        next();

        while (!end() && (current() == '_' || std::isalnum(current())))
        {
            char c = next();
            word += c;
        }

        Token::Type store_type = Token::Type::IDENTIFIER;
        if (word == "exit")        store_type = Token::Type::KEYWORD_EXIT;
        else if (word == "return") store_type = Token::Type::KEYWORD_RETURN;
        else if (word == "true")   store_type = Token::Type::KEYWORD_TRUE;
        else if (word == "false")  store_type = Token::Type::KEYWORD_FALSE;
        else if (word == "let")    store_type = Token::Type::KEYWORD_LET;
        else if (word == "func")   store_type = Token::Type::KEYWORD_FUNC;

        p_tokens_->push_back(Token{word, store_type});
    }

    // only supports integers
    void Lexer::tokenize_num()
    {
        std::string num;
        bool is_float = false;

        while (!end() && (current() == '.' || std::isdigit(current())))
        {
            char c = next();
            num += c;

            if (c == '.')
            {
                if (is_float)
                {
                    raiseError("Extra dot in float literal");
                }

                is_float = true;

                continue;
            }
        }

        if (num.at(num.size() - 1) == '.')
        {
            raiseError("Float literal cannot terminate with '.'");
        }

        p_tokens_->push_back(Token{num, is_float ? Token::Type::LITERAL_FLOAT : Token::Type::LITERAL_INT});
    }

    void Lexer::tokenize_symbol()
    {
        /*
            Separators:
            - ';'
            - ':'
            - ','
            - '.'
            - '('
            - ')'
            - '['
            - ']'
            - '{'
            - '}'
        */

        // we first check for separator

        Token seperator {"", Token::Type::END_OF_FILE};
        char c = current();
        switch (c)
        {
            case ';': seperator = {";", Token::Type::SEMICOLON};        break;
            case ':': seperator = {":", Token::Type::COLON};            break;
            case ',': seperator = {",", Token::Type::COMMA};            break;
            case '.': seperator = {".", Token::Type::DOT};              break;
            case '(': seperator = {"(", Token::Type::LEFT_PAREN};       break;
            case ')': seperator = {")", Token::Type::RIGHT_PAREN};      break;
            case '[': seperator = {"[", Token::Type::LEFT_BRACKET};     break;
            case ']': seperator = {"]", Token::Type::RIGHT_BRACKET};    break;
            case '{': seperator = {"{", Token::Type::LEFT_BRACE};       break;
            case '}': seperator = {"}", Token::Type::RIGHT_BRACE};      break;
        }

        // value not being empty means one case triggered
        if (!seperator.value.empty())
        {
            p_tokens_->push_back(seperator);
            next();
            return;
        }

        /*
            Operators:
            - '+'
            - '-'
            - '*'
            - '/'
            - '%'
            - '='
        */

        next();

        // we use if statements to handle multi-character
        // operators. not needed right now though
        if (c == '+')
        {
            p_tokens_->push_back(Token{"+", Token::Type::OPERATOR_PLUS});
            return;
        }

        if (c == '-')
        {
            p_tokens_->push_back(Token{"-", Token::Type::OPERATOR_MINUS});
            return;
        }

        if (c == '*')
        {
            p_tokens_->push_back(Token{"*", Token::Type::OPERATOR_STAR});
            return;
        }

        if (c == '/')
        {
            p_tokens_->push_back(Token{"/", Token::Type::OPERATOR_SLASH});
            return;
        }

        if (c == '%')
        {
            p_tokens_->push_back(Token{"%", Token::Type::OPERATOR_PERCENT});
            return;
        }

        if (c == '=')
        {
            p_tokens_->push_back(Token{"=", Token::Type::OPERATOR_ASSIGNMENT});
            return;
        }

        raiseError("Unknown operator symbol " + std::string(1, c));
    }

    void Lexer::tokenize_string()
    {
        next(); // consume "
        std::string string;

        while (!end() && current() != '"')
        {
            char c = next();
            if (c != '\\')
            {
                string += c;
                continue;
            }

            else 
            {
                char escape_character = next();
                char intended_character;

                switch (escape_character)
                {
                    case 'n': intended_character = '\n'; break;
                    case 't': intended_character = '\t'; break;
                    case 'r': intended_character = '\r'; break;
                    case '0': intended_character = '\0'; break;
                    case 'b': intended_character = '\b'; break;
                    case 'f': intended_character = '\f'; break;
                    case 'a': intended_character = '\a'; break;
                    case 'v': intended_character = '\v'; break;

                    case '\\': intended_character = '\\'; break;
                    case '"': intended_character = '"'; break; // different for strings and chars

                    default: 
                        raiseError("Unknown escape character '" + std::string(1, escape_character) + "'");
                }

                string += intended_character;
                
                if (intended_character == '\0')
                {
                    raiseWarning("Null character in string literal can result in the literal ending at the character");
                }
            }
        }

        if (end())
        {
            raiseError("Unterminated string literal");
        }

        next(); // consume closing quote
        p_tokens_->push_back(Token{string, Token::Type::LITERAL_STRING});
    }

    void Lexer::tokenize_char()
    {
        next(); // consume '

        if (current() == '\\')
        {
            next(); // consume '\'

            if (current() == '\'')
            {
                raiseError("Empty character literal");
            }

            char escape_character = next();
            char intended_character;

            switch (escape_character)
            {
                case 'n': intended_character = '\n'; break;
                case 't': intended_character = '\t'; break;
                case 'r': intended_character = '\r'; break;
                case '0': intended_character = '\0'; break;
                case 'b': intended_character = '\b'; break;
                case 'f': intended_character = '\f'; break;
                case 'a': intended_character = '\a'; break;
                case 'v': intended_character = '\v'; break;

                case '\\': intended_character = '\\'; break;
                case '\'': intended_character = '\''; break; // different for strings and chars

                default: 
                    raiseError("Unknown escape character '" + std::string(1, escape_character) + "'");
            }

            p_tokens_->push_back(Token{std::string(1, intended_character), Token::Type::LITERAL_CHAR});
        }
        
        else 
        {
            char c = next();

            p_tokens_->push_back(Token{std::string(1, c), Token::Type::LITERAL_CHAR});
        }

        if (!expect('\'')) // expect closing quote
        {
            raiseError("Unterminated char literal");
        }
    }

    void Lexer::skip_comment()
    {
        next();
        while (!end() && current() != '\n')
        {
            next();
        }

        // no need to skip \n, the main loop handles that fine
    }
}