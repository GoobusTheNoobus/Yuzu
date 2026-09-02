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

#include "frontend/parser/ast/node.hpp"
#include "frontend/sema/scope.hpp"
#include "frontend/sema/symbol.hpp"
#include <optional>
#include <unordered_map>
#include <vector>

namespace yuzu {
class Sema {
  public:
    Sema();
    void analyze(Root& root);

  private:
    void analyze_block(Block& block);

    struct Analysis {

        std::optional<std::string> type;
        VariableSymbol* assignable = nullptr; // represents an lvalue

        inline operator bool() { return type.has_value(); }
    };
    Analysis analyze(BaseNode& node);

    std::unordered_map<std::string, FunctionSymbol> functions;
    std::unordered_map<std::string, TypeSymbol> known_types; // include native types too

    std::vector<Scope> scopes;

    std::optional<std::string> ret_type;
};
} // namespace yuzu