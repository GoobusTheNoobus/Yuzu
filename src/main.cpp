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

#include "error.hpp"
#include "lexer.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

constexpr const char* VERSION = "0.1.1";

int main(int argc, char** argv)
{
    // read -> eval -> print -> loop

    if (argc <= 1)
    {
        while (true)
        {
            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);

            if (input == "quit")
                break;
        }

        return EXIT_SUCCESS;
    }
    
    std::string arg1 = argv[1];

    if (arg1 == "--version" || arg1 == "-v")
    {
        std::cout << "Yuzu" << VERSION << std::endl;

        return EXIT_SUCCESS;
    }

    std::string filename = arg1;

    // read file
    std::ifstream file(filename);
    if (!file.is_open())
    {
        yuzu::raiseError("Cannot open source file '" + filename + "'");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string file_content = buffer.str();

    yuzu::Lexer lexer(file_content);
    auto tokens = lexer.tokenize();

    std::cout << tokens << std::endl;
}