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
#include <unordered_map>
#include "astnode.hpp"
#include "nodetypes.hpp"
#include "tokenstream.hpp"

namespace elsix{

// Forward declarations.
class TokenStream;
class ErrorHandler;

class parser{
public:
    explicit parser(TokenStream &&token_stream);
    explicit parser(TokenStream &&token_stream, ErrorHandler &&error_handler);
    ~parser() = default;
    
    ASTNode_sp parse();

private:
    TokenStream &token_stream_;
    std::unique_ptr<ErrorHandler> error_handler_;
    // A map between labels and their lines.
    std::unordered_map<std::string_view, ASTNode_sp> labels_;
    // GoTo statements that need to be back-patched.
    std::vector<ASTNode_sp> back_patch_stack;
    
    // Parse functions.
    [[nodiscard]] ASTNode_sp parse_line();
    [[nodiscard]] ASTNode_sp parse_label_or_keyword();
    [[nodiscard]] ASTNode_sp parse_if();
    [[nodiscard]] ASTNode_sp parse_test();
    [[nodiscard]] ASTNode_sp parse_then();
    [[nodiscard]] ASTNode_sp parse_goto();
    [[nodiscard]] ASTNode_sp parse_goto(ASTNode_sp label);
    [[nodiscard]] ASTNode_sp parse_operation();
    
    // Utility functions.
    void check_node_type(const ASTNode &node, NodeType expected);
    void expect(NodeType expected);
    void interpret_as_type(ASTNode_sp &node, ArgType type);
    void interpret_as_number(ASTNode_sp &node, int base = 10);
    
};

}
