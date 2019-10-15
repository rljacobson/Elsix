/*
    Created by Robert Jacobson on 10 September 2019.

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

/**
 * @brief Defines the `Location` and `Span` (range) structures to identify points and regions in a
 * source file.
 */

#pragma once

namespace elsix{

/**
 * @brief A `Location` is a human-readable (row, column) pair specifying a location in the source
 * file. Locations correspond 1-1 to buffer offsets.
 */
struct Location {
    Location() = default;
    explicit Location(int row, int column) : row(row), column(column){
    }
    Location(const Location &other) = default;

    Location &operator=(const Location &rhs) = default;

    bool operator==(const Location &rhs){
        return (row == rhs.row) && (column == rhs.column);
    };

    // Row is an index into the `SourceFile.lines` `std::vector`.
    unsigned int row = 0;
    // Column is an index into the `SourceFile.lines[row]` `std::string_view`.
    int column = 0;
};

/**
 * @brief A span is a region in a source file specified by to `Locations` (start and end).
 *
 * Spans are in 1-1 relationship with `std::string_view`s but are human readable. They can be
 * converted to `std::string_view`s with `SourceFile.get_text()`.
 */
struct Span {
    Span() = default;
    explicit Span(const Location &start, const Location &end) : start(start), end(end){
    }
    explicit Span(const Location &start) : start(start), end(Location()){
    }

    Location start;
    Location end;
};

}
