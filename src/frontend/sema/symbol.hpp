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

#include <string>
#include <vector>

namespace yuzu {

struct BaseSymbol {
    std::string name;

    BaseSymbol(const std::string& name) : name(name) {}
};

struct VariableSymbol : BaseSymbol {
    std::string type;

    VariableSymbol(const std::string& name, const std::string& type)
        : BaseSymbol(name), type(type) {}
};

struct FunctionSymbol : BaseSymbol {
    std::vector<std::string> params;
    std::string type;

    FunctionSymbol(const std::string& name, const std::vector<std::string>& params,
                   const std::string& type)
        : BaseSymbol(name), params(params), type(type) {}
};

struct TypeSymbol : BaseSymbol {

    TypeSymbol(const std::string& name) : BaseSymbol(name) {}
};

} // namespace yuzu