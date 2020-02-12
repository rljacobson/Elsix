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

#pragma once

#include <algorithm>
#include <cctype>
#include <locale>

/**
 * @brief Split the given string on the delimiter.
 * @param s: The string to split.
 * @param delimiter : The delimiter of the tokens
 * @return : A new vector of strings.
 */
[[nodiscard]] std::vector<std::string> split(const std::string &s, char delimiter) noexcept;

/**
 * @brief Trim from start (in place)
 * @param s
 */
inline void ltrim(std::string &s) noexcept;

/**
 * @brief trim from end (in place)
 * @param s
 */
inline void rtrim(std::string &s) noexcept;

/**
 * @brief trim from both ends (in place)
 * @param s
 */
inline void trim(std::string &s) noexcept;

/**
 * @brief trim from start (copying)
 * @param s
 * @return
 */
[[nodiscard]] std::string ltrim_copy(std::string s) noexcept;

/**
 * @brief trim from end (copying)
 * @param s
 * @return
 */
[[nodiscard]] std::string rtrim_copy(std::string s) noexcept;

/**
 * @brief trim from both ends (copying)
 * @param s
 * @return
 */
[[nodiscard]] std::string trim_copy(std::string s) noexcept;

// region: Character classes.

[[nodiscard]] bool isAlpha(char c) noexcept;
[[nodiscard]] bool isAlphanumeric(char c) noexcept;

/**
 * @brief Detects if the given character is an allowable Hollerith literal.
 *
 * Hollerith literals can contain digits, letters, and '.' (period).
 *
 * @param c
 * @return
 */
[[nodiscard]] bool isHollerith(char c) noexcept;

[[nodiscard]] bool isDigit(char c) noexcept;
[[nodiscard]] bool isOctal(char c) noexcept;
[[nodiscard]] bool isHex(char c) noexcept;

/**
 * @brief Detects non-newline whitespace.
 *
 * A "blank" is a character in the locale that separates words.
 * It includes only ' ' and '\t'.
 *
 * @param c
 * @return
 */
[[nodiscard]] bool isBlank(char c) noexcept;

/**
 * @brief Detects newline characters.
 *
 * A "blank" is a character in the locale that separates words and includes
 * ' ' and '\t'. A "whitespace" character additionally includes:
 * 	'\n' newline (LF)
 * 	'\v'	(0x0b)	vertical tab (VT)
 * 	'\f'	(0x0c)	feed (FF)
 * 	'\r'	(0x0d)	carriage return (C)
 *
 * @param c
 * @return
 */
[[nodiscard]] bool isNewline(char c) noexcept;
// endregion

// region: Word classes.

[[nodiscard]] bool isNumber(const std::string &name);
[[nodiscard]] bool isOctal(const std::string &name);

/**
 * @brief Detects if the given string is an allowable Hollerith literal.
 *
 * Hollerith literals can contain digits, letters, and '.' (period).
 *
 * @param c
 * @return
 */
[[nodiscard]] bool isHollerith(const std::string &name);

// endregion: word classes.
