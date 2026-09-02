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

#include "frontend/sema/scope.hpp"
#include "frontend/sema/symbol.hpp"

namespace yuzu {

VariableSymbol* Scope::lookup(const std::string& name) {

    auto it = locals.find(name);

    if (it == locals.end())
        return nullptr;

    return &it->second;
}
Scope::Scope(const std::vector<VariableSymbol>& params) {
    for (auto& symbol : params) {
        locals.emplace(symbol.name, symbol);
    }
}
void Scope::define(VariableSymbol symbol) { locals.emplace(symbol.name, symbol); }

} // namespace yuzu