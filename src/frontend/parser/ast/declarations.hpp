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
#include <memory>
#include <optional>

namespace yuzu {

struct Let : public Declaration {
    std::string name;
    std::string type;
    std::optional<std::unique_ptr<Expression>> value;

    Let(const std::string& name, const std::string& type,
        std::optional<std::unique_ptr<Expression>>& value)
        : Declaration(NodeKind::LET), name(name), type(type), value(std::move(value)) {}
};

struct Func : public Declaration {
    struct Param {
        std::string type;
        std::string name;
    };

    std::string name;
    std::string type;
    std::vector<Param> params;
    std::unique_ptr<Block> body;

    Func(const std::string& name, const std::string& type, std::vector<Param>& params,
         std::unique_ptr<Block>& body)
        : Declaration(NodeKind::FUNC), name(name), type(type), params(params),
          body(std::move(body)) {}
};

} // namespace yuzu