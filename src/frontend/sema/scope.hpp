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

#include "frontend/sema/symbol.hpp"
#include <unordered_map>
#include <vector>

namespace yuzu {
class Scope {
  public:
    Scope() = default;
    Scope(const std::vector<VariableSymbol>& params);

    VariableSymbol* lookup(const std::string& name);
    void define(VariableSymbol symbol);

  private:
    std::unordered_map<std::string, VariableSymbol> locals;
};
} // namespace yuzu