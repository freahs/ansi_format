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

         static size_t cache_size = 5;

         namespace __impl {

             /**
              * Abstract class which all stream manipulators which must implement using CRTP. This primary
              * template implements a basic LRU cache of the current state of the cache_size most resently
              * used ostreams. A cache is needed to reduce memory footprint and CRTP is used to provide a
              * unique cache for each derived class. Without a cache, care has to be taken not to flood the
              * stream with ANSI escape codes.
              * STATE_T is the type of the state stored in the cache
              */
             template<typename T, typename STATE_T = void> class stream_format {
                 static_assert(std::is_default_constructible<STATE_T>::value, "state type must be default constructable");
                 static_assert(std::is_copy_constructible<STATE_T>::value, "state type must be copy constructable");

             private:
                 using list_type = std::list<std::pair<std::ostream*, STATE_T>>;
                 using list_iterator = typename list_type::const_iterator;
                 using map_type = std::unordered_map<std::ostream*, list_iterator>;
                 static list_type s_cache_list;
                 static map_type s_cache_map;
                 STATE_T m_state;

                 bool cache_contains(std::ostream* os) const {
                     return s_cache_map.find(os) != s_cache_map.end();
                 }

                 const STATE_T& cache_get(std::ostream* os) const {
                     assert(cache_contains(os));
                     s_cache_list.splice(
                         s_cache_list.begin(),
                         s_cache_list,
                         s_cache_map.find(os)->second);
                     return s_cache_list.front().second;
                 }

                 void cache_put(std::ostream* os, const STATE_T& data) const {
                     s_cache_list.push_front({os, data});
                     s_cache_map[os] = s_cache_list.begin();
                     if (s_cache_map.size() > cache_size) {
                         auto e = std::prev(s_cache_list.end());
                         s_cache_map.erase(e->first);
                         s_cache_list.erase(e);
                     }
                 }

             protected:

                 // The actual manipulation of the stream should take place here.
                 // state is the state given to the constructor that should be applied to
                 // the stream (in some manner) 
                 // Note that this method will not be called if the state is the same as the one
                 // in the cache.
                 virtual void apply(std::ostream& os, const STATE_T& state) const = 0;

             public:
                 // The constructor takes a state that should be applied to the ostream.
                 stream_format(const STATE_T& state) : m_state(state) { }
                 stream_format() = delete;
                 stream_format(const stream_format&) = delete;

                 // Some manipulators might affecte the state of others for which the cache
                 // can be invalidated with this method.
                 static void invalidate(std::ostream& os) {
                     auto it = s_cache_map.find(&os);
                     if (it != s_cache_map.end()) {
                         s_cache_list.erase(it->second);
                         s_cache_map.erase(it);
                     }
                 }

                 std::ostream& operator()(std::ostream& os) const {
                     apply(os);
                     return os;
                 }

                 // the stream operator saves the state of the stream, applies the manipulator
                 // and then restores the state IF the manipulator state is not already in
                 // effect.
                 friend std::ostream& operator<<(std::ostream& os, const stream_format& sf) {
                     if (!sf.cache_contains(&os) || sf.cache_get(&os) != sf.m_state) {
                         static const std::ios null_state(NULL);
                         std::ios state(NULL);
                         state.copyfmt(os);
                         os.copyfmt(null_state);
                         sf.apply(os, sf.m_state);
                         os.copyfmt(state);
                         sf.cache_put(&os, sf.m_state);
                     }
                     return os;
                 }
             };

             template<typename T, typename STATE_T>
             typename stream_format<T, STATE_T>::map_type stream_format<T, STATE_T>::s_cache_map = {};

             template<typename T, typename STATE_T>
             typename stream_format<T, STATE_T>::list_type stream_format<T, STATE_T>::s_cache_list = {};

             /*
              * Partial specialication without cache. Some manipulators (like relative
              * positioning) won't work properly with a cache.
              */
             template<typename T> class stream_format<T, void> {
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

             // Binary stream manipulator for some binary state that can be turned on or off.
             template<int ON, int OFF> class binary : public stream_format<binary<ON, OFF>, bool> {
             public:
                 using stream_format<binary<ON, OFF>, bool>::stream_format;
                 void apply(std::ostream& os, const bool& state) const override {
                     os << "\033[";
                     if (state)  { os << ON; }
                     else        { os << OFF; }
                     os << "m";
                 }
             };

             // Sets either the text color or background color to a xterm_256 (8-bit) color.
             // CODE = 38 for text, CODE = 48 for background.
             template <int CODE> struct color_8 : public stream_format<color_8<CODE>, uint8_t> {
                 using stream_format<color_8<CODE>, uint8_t>::stream_format;
                 void apply(std::ostream& os, const uint8_t& state) const override {
                     os << "\033[" << CODE << ";5;" << static_cast<int>(state) << "m";
                 }
             };

             // Sets either the text color or background color to a true color (24-bit) color.
             // CODE = 38 for text, CODE = 48 for background.
             template <int CODE> struct color_24 : public stream_format<color_24<CODE>, std::array<uint8_t, 3>> {
                 color_24(uint8_t r, uint8_t g, uint8_t b) : stream_format<color_24<CODE>, std::array<uint8_t, 3>>({r, g, b}) { }
                 void apply(std::ostream& os, const std::array<uint8_t, 3>& state) const override {
                     uint8_t r = state[0];
                     uint8_t g = state[1];
                     uint8_t b = state[2];
                     os << "\033[" << CODE << ";2;" << static_cast<int>(r) << ";" << static_cast<int>(g) << ";" << static_cast<int>(b) << "m";
                 }
             };

             // Resets either the text color or the background color to the default terminal color.
             // CODE = 38 for text, CODE = 48 for background.
             template <int CODE> struct color_default : public stream_format<color_default<CODE>> {
                 void apply(std::ostream& os) const override {
                     os << "\033[" << CODE << "m";
                     color_8<CODE-1>::invalidate(os);
                     color_24<CODE-1>::invalidate(os);
                 }
             };
         }

         // Positions the cursor at the position specified. 1,1 is in the top left corner.
         struct pos : public __impl::stream_format<pos, std::pair<int, int>> {
             pos(int row, int col) : stream_format<pos, std::pair<int, int>>({row, col}) { }
             void apply(std::ostream& os, const std::pair<int, int>& state) const override {
                 int row = state.first;
                 int col = state.second;
                 os << "\033[" << row << ";" << col <<  "H";
             }
         };

         // positions the cursor at the position specified, relative to it's current position.
         // -1,-1 is to the left and above the current position, while 1, 1 is to the right and
         // below.
         class rpos : public __impl::stream_format<rpos> {
             int m_row, m_col;
         public:
             rpos(int row, int col) : m_row(row), m_col(col) { }
             void apply(std::ostream& os) const override {
                 if (m_row < 0) { os << "\033[" << -m_row << "A"; }
                 if (m_row > 0) { os << "\033[" << m_row << "B"; }
                 if (m_col < 0) { os << "\033[" << -m_col << "D"; }
                 if (m_col > 0) { os << "\033[" << m_col << "C"; }
                 pos::invalidate(os);
             }
         };

         // Hides the cursor.
         struct hide : public __impl::stream_format<hide, bool> {
             using stream_format<hide, bool>::stream_format;
             void apply(std::ostream& os, const bool& state) const override {
                 if (state)  { os << "\033[?25l"; }
                 else        { os << "\033[?25h"; }
             }
         };

         // Common formats
         typedef __impl::binary<1, 22> bold;
         typedef __impl::binary<3, 23> italic;
         typedef __impl::binary<4, 24> underline;

         // Text (foreground) and background color
         inline __impl::color_8<38> fg(uint8_t c) { return {c}; }
         inline __impl::color_24<38> fg(uint8_t r, uint8_t g, uint8_t b) { return {r, g, b}; }
         inline __impl::color_8<48> bg(uint8_t c) { return {c}; }
         inline __impl::color_24<48> bg(uint8_t r, uint8_t g, uint8_t b) { return {r, g, b}; }

         // Apply default text color.
         inline std::ostream& fg_default(std::ostream& os) {
             return os << __impl::color_default<39>();
         }

         // Apply default background color.
         inline std::ostream& bg_default(std::ostream& os) {
             return os << __impl::color_default<49>();
         }

         // clear funtion
         inline std::ostream& clear(std::ostream& os) {
             os << "\033[0m";
             return os;
         }

     }

#endif
