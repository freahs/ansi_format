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


#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <random>
#include <vector>
#include "../inc/ansi_format.hpp"

static const float pi = 3.141592;

struct color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    int intensity;
    color() = default;
    color(std::uint8_t r, std::uint8_t g, std::uint8_t b) : r(r), g(g), b(b), intensity(r*g*b) { }
    friend std::ostream& operator<<(std::ostream& os, const color& c) {
        return os << "(" << static_cast<int>(c.r) << "," << static_cast<int>(c.g) << "," << static_cast<int>(c.b) << ")";
    }
    bool operator==(const color& that) {
        return this->r == that.r && this->g == that.g && this->b == that.b;
    }
    bool operator<(const color& that) {
        return this->intensity < that.intensity;
    }
};


struct plasma {
    int m_rows, m_cols;
    plasma(int rows, int cols) : m_rows(rows), m_cols(cols) { }

    static int get_seed() {
        static std::random_device srd;
        static std::mt19937 mt(srd());
        auto dist = std::uniform_int_distribution<int>(3500, 100000);
        auto x = dist(mt);
        return x;
    }

    long run(int dur, int seed) {
        std::cout << format::clear;
        auto init = std::chrono::high_resolution_clock::now();
        int elapsed = 0;
        int num_cycles = 0;

        std::cout << format::hide(true);
        while(elapsed < dur) {
            elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - init).count();
            auto t = (elapsed + seed)/3500.0;

            for (int r = 0; r < m_rows; ++r) {
                for (int c = 0; c < m_cols; ++c) {
                    float _y = r/static_cast<float>(m_rows*2);
                    float _x = c/static_cast<float>(m_cols*2);

                    float v1 = sin(_x*5.0 + t);
                    float v2 = sin(5.0*(_x*sin(t / 2.0) + _y*cos(t/3.0)) + t);
                    float cx = _x + sin(t / 5.0)*5.0;
                    float cy = _y + sin(t / 3.0)*5.0;
                    float v3 = sin(sqrt(100.0*(cx*cx + cy*cy)) + t);

                    float vf = v1 + v2 + v3;
                    float r  = cos(vf*pi);
                    float g  = sin(vf*pi + 6.0*pi/3.0);
                    float b  = cos(vf*pi + 4.0*pi/3.0);

                    std::cout << format::bg(
                        static_cast<uint8_t>(r < 0 ? 0 : r*255),
                        static_cast<uint8_t>(g < 0 ? 0 : g*255),
                        static_cast<uint8_t>(b < 0 ? 0 : b*255)
                        ) << " ";
                }
                std::cout << format::bg(-1) << std::endl;
            }
            std::cout << format::rpos(-m_rows, 0);
            ++num_cycles;
        }

        std::cout << format::hide(true);
        std::cout << format::bg_default << format::fg_default;
        for (int r = 0; r < m_rows; ++r) {
            for (int c = 0; c < m_cols; ++c) {
                std::cout << " ";
            }
            std::cout << std::endl;
        }
        std::cout << format::rpos(-m_rows, 0);
        std::cout << format::hide(false);
        return num_cycles;
    }
};

int main(int argc, char* argv[]) {
    int d = 4000; // 4 seconds
    auto s = plasma::get_seed();
    int h = 30;
    int w = 120;
    bool t = false;

    for (int i = 1; i < argc; ++i) {
        auto flag = std::string(argv[i]);
        if (flag == "-d" || flag == "--duration") { d = std::atoi(argv[++i]); }
        if (flag == "-s" || flag == "--seed")     { s = std::atoi(argv[++i]); }
        if (flag == "-w" || flag == "--width")    { w = std::atoi(argv[++i]); }
        if (flag == "-h" || flag == "--height")   { h = std::atoi(argv[++i]); }
        if (flag == "-t" || flag == "--timeit")   { t = true; }
    }

    plasma p(h, w);
    auto c = p.run(d, s);
    if (t) {
        std::cout << "seed " << s << " took " << c << " cycles" << std::endl;
    }
}
