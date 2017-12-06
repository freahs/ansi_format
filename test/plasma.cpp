#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <thread>
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
    std::vector<std::vector<color>> m_colors;
    plasma(int rows, int cols) : m_rows(rows), m_cols(cols), m_colors(rows+2, std::vector<color>(cols+2)) { }

    void set_colors(float t) {
        for (int row = 0; row < m_rows + 2; ++row) {
            for (int col = 0; col < m_cols + 2; ++col) {
                float _y = row/static_cast<float>(m_rows*2 + 2);
                float _x = col/static_cast<float>(m_cols*2 + 2);

                float v1 = sin(_x*5.0 + t);
                float v2 = sin(5.0*(_x*sin(t / 2.0) + _y*cos(t/3.0)) + t);
                float cx = _x + sin(t / 5.0)*5.0;
                float cy = _y + sin(t / 3.0)*5.0;
                float v3 = sin(sqrt(100.0*(cx*cx + cy*cy)) + t);

                float vf = v1 + v2 + v3;
                float r  = cos(vf*pi);
                float g  = sin(vf*pi + 6.0*pi/3.0);
                float b  = cos(vf*pi + 4.0*pi/3.0);

                get_color(row, col) = color(
                    static_cast<uint8_t>(r < 0 ? 0 : r*255),
                    static_cast<uint8_t>(g < 0 ? 0 : g*255),
                    static_cast<uint8_t>(b < 0 ? 0 : b*255)
                    );
            }
        }
    }

    color& get_color(int row, int col) {
        return m_colors[row][col];
    }

    void print(int row, int col) {
        color& bg = get_color(row, col);
        std::cout << format::bg(bg.r, bg.g, bg.b) << " ";
    }

    void run (int dur) {
        run(dur, 0);
    }


    void run(int dur, int seed) {
        std::cout << format::clear;
        int i = 0;
        while(dur > 0) {
            ++i;
            auto start = std::chrono::system_clock::now();
            auto t = (i + seed)/200.0;

            set_colors(t);

            std::cout << format::hide(true);
            for (int r = 0; r < m_rows; ++r) {
                for (int c = 0; c < m_cols; ++c) {
                    print(r, c);
                }
                std::cout << format::bg_default << "\n";
            }
            std::cout << std::endl;
            std::cout << format::rpos(-m_rows - 1, 0);
            std::cout << format::hide(false);
            auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
            std::this_thread::sleep_for(std::chrono::milliseconds(25 - delta));
            dur -= 25;
        }
    }

};

int main(int argv, char** argc) {
    int dur = 4000; // 1 second
    if (argv > 1) {
        dur =  atoi(argc[1]);
    }

    plasma p(20, 80);
    p.run(dur);

}
