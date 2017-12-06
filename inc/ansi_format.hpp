/*
 * Copyright 2016 Fredrik Åhs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANSI_FORMAT_H
#define ANSI_FORMAT_H

#include <ostream>

namespace format {

    template<typename T> class stream_format {
    protected:
        virtual void apply(std::ostream& os) const = 0;

    public:
        std::ostream& operator()(std::ostream& os) const {
            apply(os);
            return os;
        }

        friend inline std::ostream& operator<<(std::ostream& os, const stream_format& sf) {
            static const std::ios null_state(NULL);
            std::ios state(NULL);
            state.copyfmt(os);
            os.copyfmt(null_state);
            sf.apply(os);
            os.copyfmt(state);
            return os;
        }
    };

    template<int ON, int OFF> class binary : public stream_format<binary<ON, OFF>> {
        bool m_status;
    public:
        binary(bool status) : m_status(status) { }
        void apply(std::ostream& os) const override {
            os << "\033[";
            if (m_status) { os << ON; }
            else          { os << OFF; }
            os << "m";
        }
    };

    template <int CODE> class color_8 : public stream_format<color_8<CODE>> {
        uint8_t m_color;
    public:
        color_8(uint8_t color) : m_color(color) { }
        void apply(std::ostream& os) const override {
            os << "\033[" << CODE << ";5;" << static_cast<int>(m_color) << "m";
        }
    };

    template <int CODE> class color_24 : public stream_format<color_24<CODE>>{
        uint8_t m_r, m_g, m_b;
    public:
        color_24(uint8_t r, uint8_t g, uint8_t b) : m_r(r), m_g(g), m_b(b) { }
        void apply(std::ostream& os) const override {
            os << "\033[" << CODE << ";2;" << static_cast<int>(m_r) << ";" << static_cast<int>(m_g) << ";" << static_cast<int>(m_b) << "m";
        }
    };

    template <int CODE> struct color_default : public stream_format<color_default<CODE>> {
        void apply(std::ostream& os) const override {
            os << "\033[" << CODE << "m";
        }
    };

    class pos : public stream_format<pos> {
        int m_row, m_col;
    public:
        pos(int row, int col) : m_row(row), m_col(col) { }
        void apply(std::ostream& os) const override {
            os << "\033[" << m_row << ";" << m_col <<  "H";
        }
    };

    class rpos : public stream_format<rpos> {
        int m_row, m_col;
    public:
        rpos(int row, int col) : m_row(row), m_col(col) { }
        void apply(std::ostream& os) const override {
            if (m_row < 0) { os << "\033[" << -m_row << "A"; }
            if (m_row > 0) { os << "\033[" << m_row << "B"; }
            if (m_col < 0) { os << "\033[" << -m_col << "D"; }
            if (m_col > 0) { os << "\033[" << m_col << "C"; }
        }
    };

    class hide : public stream_format<hide> {
        bool m_hidden;
    public:
        hide(bool hidden) : m_hidden(hidden) { }
        void apply(std::ostream& os) const override {
            if (m_hidden) { os << "\033[?25l"; }
            else          { os << "\033[?25h"; }
        }
    };

    // Common formats
    typedef binary<1, 22> bold;
    typedef binary<3, 23> italic;
    typedef binary<4, 24> underline;

    // Foreground and background color
    inline color_8<38> fg(uint8_t c) { return color_8<38>(c); }
    inline color_24<38> fg(uint8_t r, uint8_t g, uint8_t b) { return color_24<38>(r, g, b); }
    inline color_8<48> bg(uint8_t c) { return color_8<48>(c); }
    inline color_24<48> bg(uint8_t r, uint8_t g, uint8_t b) { return color_24<48>(r, g, b); }

    inline std::ostream& fg_default(std::ostream& os) {
        return os << color_default<39>();
    }

    inline std::ostream& bg_default(std::ostream& os) {
        return os << color_default<49>();

    }

    // clear funtion
    inline std::ostream& clear(std::ostream& os) {
        os << "\033[0m";
        return os;
    }

}

#endif
