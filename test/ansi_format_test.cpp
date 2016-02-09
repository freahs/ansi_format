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
