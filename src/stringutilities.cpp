/*
    Created by Robert Jacobson on 18 May 2019.

    Elsix
    Description: An implementation of the Bell Telephone Laboratories'
                 Low-Level Linked List Language L6.

    Copyright (c) 2019 Robert Jacobson.
        The MIT License

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.

    */

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "stringutilities.hpp"

namespace elsix{

// region: split and trim

template<typename Out>
void split(const std::string &s, char delim, Out result){
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)){
        *(result++) = item;
    }
}

inline std::vector<std::string> split(const std::string &s, char delimiter) noexcept{
    std::vector<std::string> elems;
    split(s, delimiter, std::back_inserter(elems));
    return elems;
}

// trim from start (in place)
inline void ltrim(std::string &s) noexcept{
    s.erase(
        s.begin(), std::find_if(
            s.begin(), s.end(), [](int ch){
                return !std::isspace(ch);
            }
        ));
}

// trim from end (in place)
inline void rtrim(std::string &s) noexcept{
    s.erase(
        std::find_if(
            s.rbegin(), s.rend(), [](int ch){
                return !std::isspace(ch);
            }
        ).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) noexcept{
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
[[nodiscard]] inline std::string ltrim_copy(std::string s) noexcept{
    ltrim(s);
    return s;
}

// trim from end (copying)
[[nodiscard]] inline std::string rtrim_copy(std::string s) noexcept{
    rtrim(s);
    return s;
}

// trim from both ends (copying)
[[nodiscard]] inline std::string trim_copy(std::string s) noexcept{
    trim(s);
    return s;
}
// endregion split and trim

// region: Character classes.

[[nodiscard]] bool inline isBug(char c) noexcept{
    return isalpha(c);
}

[[nodiscard]] bool inline isAlphanumeric(char c) noexcept{
    return isalnum(c);
}

/**
 * @brief Determines if the given character is an allowable Hollerith literal.
 *
 * The permitted Hollerith characters are digits, letters, and '.' (period).
 *
 * @param c : A character to check.
 * @return True for False.
 */
[[nodiscard]] bool inline isHollerith(char c) noexcept{
    return isalnum(c) || c == '.';
}

[[nodiscard]] bool inline isDigit(char c) noexcept{
    return isdigit(c);
}

[[nodiscard]] bool inline isOctal(char c) noexcept{
    return isdigit(c) && '8' > c;
}

[[nodiscard]] bool inline isHex(char c) noexcept{
    return isxdigit(c);
}

[[nodiscard]] bool inline isBlank(char c) noexcept{
    return isblank(c);
}

[[nodiscard]] bool inline isNewline(char c) noexcept{
    return isspace(c) && !isblank(c);
}
// endregion

// region: word classes

[[nodiscard]] bool inline isNumber(const std::string &name){
    return std::all_of(
        name.begin(), name.end(), [](char c){ return std::isdigit(c); }
    );
}

[[nodiscard]] bool inline isAlphabetic(const std::string &name){
    return std::all_of(
        name.begin(), name.end(), [](char c){ return std::isalpha(c); }
    );
}

[[nodiscard]] bool inline isHollerith(const std::string &name){
    return std::all_of(
        name.begin(), name.end(), [](char c){ return isHollerith(c); }
    );
}

[[nodiscard]] bool inline isOctal(const std::string &name){
    return std::all_of(
        name.begin(), name.end(), [](char c){ return (0 <= c) && (8 >= c); }
    );
}
// endregion: word classes

}
