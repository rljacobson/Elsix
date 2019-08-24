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
#include <vector>
#include "ast.hpp"

namespace elsix{

// Forward declarations.
class Tokenizer;
class ErrorHandler;

class parser{
public:
    explicit parser(Tokenizer &&token_stream);
    explicit parser(Tokenizer &&token_stream, ErrorHandler &&error_handler);
    ~parser() = default;

    shared_ASTNode parse();

private:
    Tokenizer &tokenstream_;
    std::unique_ptr<ErrorHandler> error_handler_;
    // A map between labels and their lines.
    std::unordered_map<const std::string *, shared_ASTNode> labels;
    // GoTo statements that need to be back-patched.
    std::vector<shared_ASTNode> label_gotos;

    // Parse functions.
    [[nodiscard]] shared_ASTNode parseLine();
    [[nodiscard]] shared_ASTNode parseLabelOrKeyword();
    [[nodiscard]] shared_ASTNode parseIf();
    [[nodiscard]] shared_ASTNode parseTest();
    [[nodiscard]] shared_ASTNode parseThen();
    [[nodiscard]] shared_ASTNode parseGoTo(shared_ASTNode label);
    [[nodiscard]] shared_ASTNode parseOperation();

    // Utility functions.
    void checkNodeType(const ASTNode &node, NodeType expected);
    void expect(NodeType expected);
    void interpretAsType(shared_ASTNode &node, ArgType type);
    void interpretAsNumber(shared_ASTNode &node, int base = 10);

};

}
