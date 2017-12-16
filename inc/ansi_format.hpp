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

#ifndef ANSI_FORMAT_H
#define ANSI_FORMAT_H

#include <array>
#include <ostream>
#include <type_traits>
#include <list>
#include <unordered_map>
#include <cassert>
#include <iostream>


namespace format {

    namespace detail {

        struct color_8 { static constexpr bool use_ios_xalloc = true; };
        struct color_24 { static constexpr bool use_ios_xalloc = true; };
        struct binary {static constexpr bool use_ios_xallox = true; };
        struct pos { static constexpr bool use_ios_xalloc = true; };
        struct rpos { static constexpr bool use_ios_xalloc = false; };
        struct hide { static constexpr bool use_ios_xalloc = true; };

        template<typename, int...> struct output_formater {
            static void apply(std::ostream& os, int v);
        };

        template<class C, int...INTS> class stream_output {
            static const int xid;
            const int v;

            template<class Q = C>
            typename std::enable_if<Q::use_ios_xalloc, void>::type apply(std::ostream& os) const {
                if (os.iword(xid) != v) {
                    auto flags = os.flags();
                    os.flags({});
                    output_formater<C, INTS...>::apply(os, v);
                    os.flags(flags);
                    os.iword(xid) = v;
                }
            }

            template<class Q = C>
            typename std::enable_if<!Q::use_ios_xalloc, void>::type apply(std::ostream& os) const {
                auto flags = os.flags();
                os.flags({});
                output_formater<C, INTS...>::apply(os, v);
                os.flags(flags);
                os.iword(xid) = v;
            }

        public:
            stream_output(int _v) : v(_v) { }
            stream_output(uint8_t r, uint8_t g, uint8_t b) : v(r << 16 | g << 8 | b) { }
            stream_output(int16_t row, int16_t col) : v(row << 16 | col) { }
            stream_output(bool b) : v(static_cast<bool>(b)) { }

            static void invalidate(std::ostream& os) {
                os.iword(xid) = -2;
            }

            friend std::ostream& operator<<(std::ostream& os, const stream_output& so) {
                so.apply(os);
                return os;
            }
        };

        template<class C, int...INTS> const int stream_output<C, INTS...>::xid = std::ios::xalloc();

        template <int CODE> struct output_formater<color_8, CODE> {
            static void apply(std::ostream& os, int v) {
                if (v == -1) { os << "\033[" << (CODE + 1) << "m"; }
                else         { os << "\033[" << CODE << ";5;" << v << "m"; }
                stream_output<color_24, CODE>::invalidate(os);
            }
        };

        template <int CODE> struct output_formater<color_24, CODE> {
            static void apply(std::ostream& os, int v) {
                os << "\033[" << CODE << ";2;" <<
                static_cast<int>(v >> 16 & 0xFF) << ";" <<
                static_cast<int>(v >> 8 & 0xFF) << ";" <<
                static_cast<int>(v & 0xFF) << "m";
                stream_output<color_8, CODE>::invalidate(os);
            }
        };

        template <int ON, int OFF> struct output_formater<binary, ON, OFF> {
            static void apply(std::ostream& os, int v) {
                if (static_cast<bool>(v)) { os << "\033[" << ON << "m"; }
                else                      { os << "\033[" << OFF  << "m"; }
            }
        };

        template <> struct output_formater<pos> {
            static void apply(std::ostream& os, int v) {
                os << "\033[" <<
                static_cast<int16_t>(v >> 16) << ";" <<
                static_cast<int16_t>(v & 0xFFFF) << "H";
                stream_output<color_8, 38>::invalidate(os);
                stream_output<color_8, 48>::invalidate(os);
                stream_output<color_24, 38>::invalidate(os);
                stream_output<color_24, 48>::invalidate(os);
            }
        };

        template <> struct output_formater<rpos> {
            static void apply(std::ostream& os, int v) {
                auto row = static_cast<int16_t>(v >> 16);
                auto col = static_cast<int16_t>(v & 0xFFFF);
                if (row < 0) { os << "\033[" << -row << "A"; }
                if (row > 0) { os << "\033[" << row << "B"; }
                if (col < 0) { os << "\033[" << -col << "D"; }
                if (col > 0) { os << "\033[" << col << "C"; }
                stream_output<color_8, 38>::invalidate(os);
                stream_output<color_8, 48>::invalidate(os);
                stream_output<color_24, 38>::invalidate(os);
                stream_output<color_24, 48>::invalidate(os);
            }
        };

        template <> struct output_formater<hide> {
            static void apply(std::ostream& os, int v) {
                if (static_cast<bool>(v)) { os << "\033[?25l"; }
                else                      { os << "\033[?25h"; }
            }
        };

    }

    // Sets the background color to xterm-256 color c. -1 will set the background color
    // to the default terminal color.
    inline detail::stream_output<detail::color_8, 48> bg(int c) {
        return {c};
    }

    // Sets the text color to xterm-256 color c. -1 will set the text color to the default
    // terminal color.
    inline detail::stream_output<detail::color_8, 38> fg(int c) {
        return {c};
    }

    // Sets the background color to the 24-bit true color r, g, b.
    inline detail::stream_output<detail::color_24, 48> bg(uint8_t r, uint8_t g, uint8_t b) {
        return {r,g,b};
    }

    // Sets the text color to the 24-bit true color r, g, b.
    inline detail::stream_output<detail::color_24, 38> fg(uint8_t r, uint8_t g, uint8_t b) {
        return {r,g,b};
    }

    // Positions the cursor at row, col. Coordinates are 1-based, with (1,1) in the top left corner.
    inline detail::stream_output<detail::pos> pos(int16_t row, int16_t col) {
        return {row, col};
    }

    // Positions the cursor at row, col relative to its current position. Negative values to the left
    // and above, positive to the right and below. The resulting position are clamped to the screen.
    inline detail::stream_output<detail::rpos> rpos(int16_t row, int16_t col) {
        return {row, col};
    }

    // Enables or disables bold text.
    inline detail::stream_output<detail::binary, 1, 22> bold(bool state) {
        return {state};
    }

    // Enables or disables italic text.
    inline detail::stream_output<detail::binary, 3, 23> italic(bool state) {
        return {state};
    }

    // Enables or disables underline text.
    inline detail::stream_output<detail::binary, 4, 24> underline(bool state) {
        return {state};
    }

    // Hides the cursor.
    inline detail::stream_output<detail::hide> hide(bool state) {
        return {state};
    }

    // Sets the text color to the default terminal background color.
    inline std::ostream& fg_default(std::ostream& os) {
        return os << fg(-1);
    }

    // Sets the background color to the defaul terminal background color.
    inline std::ostream& bg_default(std::ostream& os) {
        return os << bg(-1);
    }

    // Clears any formating.
    inline std::ostream& clear(std::ostream& os) {
        return os << "\033[0m";
    }
}

#endif
