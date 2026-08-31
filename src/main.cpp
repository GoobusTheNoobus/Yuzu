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

#include "core/error.hpp"
#include "frontend/lexer/lexer.hpp"
#include "frontend/parser/ast/pretty.hpp"
#include "frontend/parser/parser.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

constexpr const char* VERSION = "0.1.6";
constexpr const char* HELP_MESSAGE = "Yuzu Compiler\n"
                                     "\n"
                                     "Usage:\n"
                                     "    yuzu "
                                     "    yuzu <file>\n"
                                     "    yuzu [options] <file>\n"
                                     "\n"
                                     "Options:\n"
                                     "    -h, --help       Show this help message\n"
                                     "    -v, --version    Show compiler version\n";

using namespace yuzu;

int main(int argc, char** argv) {

    if (argc <= 1) {
        std::cout << HELP_MESSAGE << std::endl;
        return EXIT_SUCCESS;
    }

    std::string arg1 = argv[1];

    if (arg1 == "--version" || arg1 == "-v") {
        std::cout << "Yuzu " << VERSION << std::endl;
        return EXIT_SUCCESS;
    }

    else if (arg1 == "--help" || arg1 == "-h") {
        std::cout << HELP_MESSAGE << std::endl;
        return EXIT_SUCCESS;
    }

    std::string filename = std::move(arg1);

    std::ifstream file(filename);
    if (!file.is_open()) {
        yuzu::raise_error("Cannot open source file '" + filename + "'");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string file_content = buffer.str();

    Lexer lexer(file_content);
    Parser parser(lexer.tokenize());
    pretty_print(*parser.parse());
}