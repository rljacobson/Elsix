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

#include <vector>
#include <fstream>
#include "sourcefile.hpp"
#include "stringutilities.hpp"  // isNewline()

namespace elsix{

SourceFile::SourceFile(const std::string &filename): filename_(filename){
    // ToDo: Integrate with error reporting system.

    std::fstream in_stream(filename, std::ios::in | std::ios::binary);
    // Seek to the end to get the size of the file.
    in_stream.seekg(0, std::ios::end);
    // Get the file size (offset of end).
    size_t file_length = in_stream.tellg();
    // Return the input pointer to the beginning.
    in_stream.seekg(0, std::ios::beg);
    // Allocate enough memory for the file.
    buffer = std::unique_ptr<char[]>(new char[file_length]); // NOLINT(hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    // Read everything into memory `buffer`.
    in_stream.read(buffer.get(), file_length);

    // Segment the contents of the file into lines separated by '\n'. We try to over-estimate the
    // number of lines in the source file by assuming an average line length of 40 characters.
    // It's a guess, but it seems to work well.
    lines.reserve(file_length / 40);
    // Scope of i, start:
    {
        size_t i = 0;
        size_t start = 0;
        for(; i < file_length; ++i){
            if(isNewline(buffer[i])){
                lines.emplace_back(buffer.get() + start, i - start);
                start = i + 1;
            }
        }
        // Don't forget to add the last line in the case that the file does not end in '\n'.
        if(!isNewline(buffer[file_length - 1])){
            lines.emplace_back(buffer.get() + start, i - start);
        }
    }

    // Initialize `eof_` to one past the end of the last line.
    if(lines.empty()){
        eof_ = Location(0,0);
    } else {
        eof_ = Location(lines.size() - 1, lines.back().length()); // NOLINT(bugprone-narrowing-conversions)
    }
}

/**
 * @brief Returns a `std::string_view` of the span.
 * @param span: A span within the file.
 * @return: A `string_view` containing the text covered by the span.
 */
inline std::string_view SourceFile::span_to_string(Span span) const noexcept{
    const char * begin = lines[span.start.row].begin() + span.start.column;
    const char * end = lines[span.end.row].begin() + span.end.column;
    return std::string_view(begin, end - begin);
}

}
