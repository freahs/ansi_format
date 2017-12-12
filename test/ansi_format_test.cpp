/*
 * Copyright (C) 2017 Fredrik Åhs (freahs@kth.se)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../inc/ansi_format.hpp"

#include <iostream>

int main (int argc, char *argv[]) {


    std::cout << "no format" << std::endl;
    std::cout << format::bold(true) << "bold on" << std::endl;
    std::cout << format::bold(false) << "bold off" << std::endl;
    std::cout << format::underline(true) << "underline on" << std::endl;
    std::cout << format::underline(false) << "underline off" << std::endl;
    std::cout << format::italic(true) << "italic on" << std::endl;
    std::cout << format::italic(false) << "italic off" << std::endl;
    std::cout << format::bold(true) << "bold on" << std::endl;
    std::cout << format::underline(true) << "underline on" << std::endl; 
    std::cout << format::italic(true) << "italic on" << std::endl;
    std::cout << format::bold(false) << "bold off" << std::endl;
    std::cout << format::bold(true) << "bold on" << std::endl;
    std::cout << format::underline(false) << "underline off" << std::endl;
    std::cout << format::underline(true) << "underline on" << std::endl;
    std::cout << format::italic(false) << "italic off" << std::endl;
    std::cout << format::italic(true) << "italic on" << std::endl;
    std::cout << std::endl;
    std::cout << format::clear << "no format (clear)" << std::endl;
    std::cout << std::endl;

    for (int fg = 0; fg < 16; ++fg) {
        std::cout << format::fg(fg);
        for (int bg = 0; bg < 16; ++bg) {
            std::cout << format::bg(bg) << "X";
        }
        std::cout << format::clear << " ";
        for (int c = 0; c < 16; ++c) {
            std::cout << format::bg(fg*16 + c) << " ";
        }
        std::cout << format::clear << std::endl;
    }

    std::cout << std::endl;
}
