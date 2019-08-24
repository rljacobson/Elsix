/*
    Created by Robert Jacobson on 04 April 2019.

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

#include <iosfwd>
#include <string>

#include "tokens.hpp"

namespace elsix{

// Forward declarations.
class ErrorHandler;
struct Location;
struct LocationRangeImpl;
using LocationRange = std::shared_ptr<LocationRangeImpl>;
class ASTNode;
// A parent owns its children.
using shared_ASTNode = std::shared_ptr<ASTNode>;
// A child does not own its parent.
using weak_ASTNode = std::weak_ptr<ASTNode>;

class Tokenizer {
public:
    explicit Tokenizer(std::istream &in);
    explicit Tokenizer(std::istream &in, ErrorHandler &&error_handler);
    ~Tokenizer();

    // The tokenizer
    [[nodiscard]] ASTNode &peek();
    [[nodiscard]] const std::string *peekText() const noexcept;
    shared_ASTNode next();
    bool expect(char expected);
    bool expect(const std::string &expected);
    bool expect(NodeType expected);

    // The tokenizer tracks the location within the source file.
    [[nodiscard]] Location here() const noexcept;
    [[nodiscard]] LocationRange startRange() const;
    [[nodiscard]] LocationRange endRange(Location start) const;

private:
    std::istream &in_;
    int row_, column_, file_id_;
    ASTNode *current_token_;
    std::string current_line_;
    ErrorHandler &error_handler_;

    char nextNonBlank_();
    char nextChar_();
    void getPeekToken_();
    [[nodiscard]] char peekChar_();
};

}
