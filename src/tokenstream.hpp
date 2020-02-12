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
 * @brief An allocation-free lexer.
 */

#pragma once

#include <cstdint>
#include <string>

#include "error.hpp"
#include "sourcefile.hpp"
#include "nodetypes.hpp"
#include "astnode.hpp"
#include "location.hpp"

// ASCII ETX ("End of Text") character:
#define EOF_CHARACTER '\3'
#define EOL_CHARACTER '\n'

namespace elsix{

class TokenStream{
public:
    explicit TokenStream(const std::string &source_filename);
    ~TokenStream() = default;
    
    // The tokenizer
    [[nodiscard]] ASTNode *peek();
    // [[nodiscard]] const std::string *peekText() const noexcept;
    ASTNode_sp next();
    
    // The tokenizer tracks the location within the source file.
    // [[nodiscard]] Location here() const noexcept;
    // [[nodiscard]] Span startRange() const;
    // [[nodiscard]] Span endRange(Location start) const;
    
    // Exposes `SourceFile.span_to_string()`.
    [[nodiscard]] std::string_view span_to_string(Span span) const;

private:
    SourceFile source_file_;
    ErrorHandler error_handler_;
    Location cursor_;
    ASTNode *staged_token_ = nullptr;
    
    char next_nonblank_char_();
    char next_char_();
    char peek_char_();
    void tokenize_next_();
};

}
