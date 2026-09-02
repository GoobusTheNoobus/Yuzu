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

#include "frontend/sema/sema.hpp"
#include "core/error.hpp"
#include "frontend/lexer/token.hpp"
#include "frontend/parser/ast/declarations.hpp"
#include "frontend/parser/ast/expressions.hpp"
#include "frontend/parser/ast/node.hpp"
#include "frontend/parser/ast/pretty.hpp"
#include "frontend/parser/ast/statements.hpp"
#include "frontend/sema/native.hpp"
#include "frontend/sema/symbol.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace yuzu {

Sema::Sema() {

    known_types.emplace("int8", TypeSymbol("int8"));
    known_types.emplace("int16", TypeSymbol("int16"));
    known_types.emplace("int32", TypeSymbol("int32"));
    known_types.emplace("int64", TypeSymbol("int64"));

    known_types.emplace("float32", TypeSymbol("float32"));
    known_types.emplace("float64", TypeSymbol("float64"));
    known_types.emplace("char", TypeSymbol("char"));
    known_types.emplace("bool", TypeSymbol("bool"));

    known_types.emplace("string", TypeSymbol("string"));

    // global
    scopes.push_back(Scope());
}

void Sema::analyze(Root& root) {

    for (auto& child : root.children) {
        analyze(*child);
    }
}

// TODO: add checks to see if type exists
Sema::Analysis Sema::analyze(BaseNode& node) {

    switch (node.kind) {

    case yuzu::NodeKind::BLOCK: {
        scopes.push_back(Scope());
        analyze_block(static_cast<Block&>(node));
        scopes.pop_back();
        return {};
    }

    case yuzu::NodeKind::CALL: {
        auto& call = static_cast<Call&>(node);

        // change when yuzu supports calls of non-identifiers
        std::string name = static_cast<Identifier&>(*call.name).name;

        auto it = functions.find(name);

        if (it == functions.end()) {
            std::cout << functions.size() << std::endl;
            raise_error("Unknown function '" + name + "'");
        }

        auto& func = it->second;

        auto& expected_params = func.params;
        auto& gotten_params = call.arguments;

        // compare parameters
        usize expected_arg_nb = expected_params.size();
        usize gotten_arg_nb = gotten_params.size();

        if (expected_arg_nb != gotten_arg_nb) {
            raise_error("Function '" + name + "' accepts " + std::to_string(expected_arg_nb) +
                        ", gotten " + std::to_string(gotten_arg_nb) + " arguments");
        }

        for (usize i = 0; i < gotten_arg_nb; ++i) {
            auto& expected_param_type = expected_params[i];
            auto gotten_param = analyze(*(gotten_params[i]));

            if (!gotten_param) {
                raise_error("Expected an expression");
            }

            else if (gotten_param.type &&
                     !is_implicitly_convertible(*gotten_param.type, expected_param_type)) {
                raise_error("Cannot bind type " + *gotten_param.type + " to type " +
                            expected_param_type);
            }
        }

        return {func.type};
    }

    case yuzu::NodeKind::CAST: {
        auto& cast = static_cast<Cast&>(node);

        auto from_analysis = analyze(*cast.expression);

        if (!from_analysis) {
            raise_error("Expected an expression");
        }

        if (!is_explicitly_convertible(*from_analysis.type, cast.type))
            raise_error("Cannot convert type " + *from_analysis.type + " to type " + cast.type);

        return {cast.type};
    }

    case yuzu::NodeKind::EXIT: {
        auto& exit = static_cast<Exit&>(node);

        pretty_print(exit, 0);

        auto status = analyze(*exit.value);

        if (!status) {
            raise_error("Expected an expression");
        }

        else if (status.type) {
            if (!is_implicitly_convertible(*status.type, "int32"))
                raise_error("Exit code must be implicitly convertible to int32");
        }

        return {};
    }

    case yuzu::NodeKind::FUNC: {

        // std::cerr << "FUNC\n";
        // std::cerr << "ret_type address: " << &ret_type << '\n';
        // std::cerr << "ret_type has value: ";

        // if (ret_type.has_value()) {
        //     std::cerr << "yes\n";
        // } else {
        //     std::cerr << "no\n";
        // }

        if (ret_type) {
            raise_error("Nesting functions is not allowed");
        }

        auto& func = static_cast<Func&>(node);

        // args
        std::vector<std::string> types;
        std::vector<VariableSymbol> args;

        for (auto& param : func.params) {
            auto type_it = known_types.find(param.type);
            if (type_it == known_types.end())
                raise_error("Unknown type " + param.type);

            types.push_back(param.type);
            args.push_back(VariableSymbol(param.name, param.type));
        }

        ret_type = func.type;

        auto type_it = known_types.find(func.type);
        if (type_it == known_types.end())
            raise_error("Unknown type " + func.type);

        scopes.push_back(Scope(args));

        analyze_block(*func.body);

        scopes.pop_back();

        functions.emplace(func.name, FunctionSymbol(func.name, types, func.type));

        ret_type = std::nullopt;

        return {};
    }

    case yuzu::NodeKind::IDENTIFIER: {
        auto& id = static_cast<Identifier&>(node);

        // look from stack's top to bottom
        for (int i = scopes.size() - 1; i >= 0; --i) {
            Scope& scope = scopes[i];

            auto symbol = scope.lookup(id.name);

            if (symbol) {
                return {symbol->type, symbol};
            }
        }

        raise_error("Unknown identifier '" + id.name + "'");
        return {};
    }

    case yuzu::NodeKind::LET: {

        auto& let = static_cast<Let&>(node);

        /*std::cout << "name: " << let.name << '\n';
        std::cout << "type: " << let.type << '\n';
        std::cout << "has value: " << let.value.has_value() << '\n';*/

        // type resolution

        if (!let.value && let.type.empty()) {
            raise_error("Cannot infer type out of declaration");
        }

        if (!let.type.empty()) {
            auto type_it = known_types.find(let.type);
            if (type_it == known_types.end())
                raise_error("Unknown type " + let.type);
        }

        std::string store_type = let.type;

        if (let.value) {
            auto analysis = analyze(**let.value);

            if (!analysis) {
                raise_error("Expected an expression");
            }

            if (store_type.empty()) {
                store_type = *analysis.type;
            } else if (!is_implicitly_convertible(*analysis.type, store_type)) {

                raise_error("Cannot implicitly convert type " + *analysis.type + " to type " +
                            store_type);
            }
        }

        if (store_type.empty()) {
            raise_error("Cannot infer type out of declaration");
        }

        scopes.back().define(VariableSymbol(let.name, store_type));
        let.type = store_type;

        return {};
    }

    case yuzu::NodeKind::LITERAL_BOOL:
        return {"bool"};

    case yuzu::NodeKind::LITERAL_CHAR:
        return {"char"};

    case yuzu::NodeKind::LITERAL_FLOAT:
        return {"float64"};

    case yuzu::NodeKind::LITERAL_INT: {
        auto& lit = static_cast<IntLiteral&>(node);

        return {fits_in(lit.value, "int32") ? "int32" : "int64"};
    }

    case yuzu::NodeKind::LITERAL_STRING:
        return {"string"};

    case yuzu::NodeKind::OP_BINARY: {
        auto& binary = static_cast<BinaryOp&>(node);

        // pretty_print(binary, 0);

        auto left = analyze(*binary.left);
        auto right = analyze(*binary.right);

        if (!left || !right) {
            raise_error("Expected an expression");
        }

        if (binary.op == Token::Type::OPERATOR_ASSIGNMENT) {
            if (!left.assignable) {
                raise_error("Cannot assign to non-assignable value");
            }

            if (!is_implicitly_convertible(*right.type, *left.type)) {
                raise_error("Cannot assign type");
            }
        }

        auto common_type = binary_common_type(*left.type, *right.type, binary.op);

        if (!common_type) {
            raise_error("Unknown operand types");
        }

        return {*common_type};
    }

    case yuzu::NodeKind::OP_UNARY: {
        auto& unary = static_cast<UnaryOp&>(node);

        auto operand = analyze(*unary.operand);

        if (!operand) {
            raise_error("Expected expression");
        }

        auto type = unary_type(*operand.type, unary.op);

        if (!type) {
            raise_error("Unknown operand types");
        }

        return {*type};
    }

    case yuzu::NodeKind::RETURN: {
        if (!ret_type) {
            raise_error("Cannot return while not in function");
        }

        auto& ret = static_cast<Exit&>(node);

        auto value = analyze(*ret.value);

        if (!value) {
            raise_error("Expected an expression");
        }

        else if (value.type) {
            if (!is_implicitly_convertible(*value.type, *ret_type))
                raise_error("Return value must be implicitly convertible to " + *ret_type);
        }

        return {};
    }

    default:
        return {};
    }
}

void Sema::analyze_block(Block& block) {
    for (auto& child : block.children) {
        analyze(*child);
    }
}
} // namespace yuzu