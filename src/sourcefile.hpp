/*
    Created by Robert Jacobson on 4 October 2019.

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


/**
 * @brief A wrapper class for reading a source file into a memory buffer and segmenting it into
 * lines.
 *
 * The TokenStream uses SourceFile to determine the lines. TokenStream then breaks the lines into
 * tokens. The parser does not use SourceFile directly.  The advantage here is that a "line"
 * becomes just an index into the line array, and a "token" is a `std::string_view`, which is
 * just a pair of pointers into the buffer. No allocating and copying strings is necessary, and
 * lines and pointers can be passed by value.
 *
 * If the speed of reading the file is important (which it isn't), see
 * https://stackoverflow.com/a/17925143.
 *
 */

#include <string> // std::string_view
#include <vector> // std::vector
#include <memory> // std::unique_ptr

#include "location.hpp"

namespace elsix{
class SourceFile{

public:
    /// A SourceFile requires a path to a file.
    explicit SourceFile(const std::string &filename);
    
    Location eof(){
        return eof_;
    }
    
    /// References to lines (rows) within the source file. A more efficient method of storage
    /// would be to store only the end of each line, but the added code complexity is not worth it.
    /// Note that empty lines are still lines.
    std::vector<std::string_view> lines;
    
    /// Get a `string_view` from its span.
    [[nodiscard]] std::string_view span_to_string(Span span) const noexcept;
    
    const char *loc_to_ptr(Location loc){
        return lines[loc.row].begin() + loc.column;
    }
    
    /// Compute length of `Span`
    size_t span_length(Span s){
        return loc_to_ptr(s.end) - loc_to_ptr(s.start);
    }

private:
    // Retain a copy of the filename.
    std::string filename_;
    std::unique_ptr<char[]> buffer; // NOLINT(hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    // eof_ points to one past the end of the last line:
    //      `eof_ = Location(lines.size() - 1, lines.back().length());`
    Location eof_;
};
}
