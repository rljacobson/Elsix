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

#include <vector>
#include <variant>
#include <string>

#include "visitor.hpp"      // Visitor
#include "nodetypes.hpp"    // NodeType
#include "location.hpp"     // Span

namespace elsix{

class ASTNode;
// A parent owns its children.
using ASTNode_sp = std::shared_ptr<ASTNode>;
// A child does not own its parent.
using ASTNode_wp = std::weak_ptr<ASTNode>;

/**
 * @brief The node class of the abstract syntax tree. Tokens are leaf nodes and do not have their
 * own dedicated class.
 */
class ASTNode{
public:
    /**
     * @brief Creates a node of type `NodeType::UNDEFINED` with zeroed span.
     * @param t: The type of the node.
     */
    explicit ASTNode();
    /**
     * @brief Creates a node of type `t` with zeroed span.
     * @param t: The type of the node.
     */
    explicit ASTNode(NodeType t);
    /**
     * @brief Creates a zero-length node of type `t`.
     * @param t: The type of the node.
     * @param loc: The start and end of the node.
     */
    explicit ASTNode(NodeType t, Location loc);
    explicit ASTNode(NodeType t, Span spn);
    ~ASTNode();

    /**
     * @brief The location in the source file.
     */
    Span span;

    /**
     * @brief What the node represents.
     */
    NodeType type;

    /**
     * @brief A parent owns its children.
     */
    std::vector<ASTNode_sp> children;

    /**
     * @brief A child does not own its parent.
     */
    ASTNode_wp parent;

    /**
     * @brief Holds a numeric value, a (pointer to) a string, or a pointer to a block.
     *
     */
    using ValueType = std::variant<unsigned long, ASTNode_wp, const std::string_view>;
    ValueType value;

    [[nodiscard]] const std::string_view value_as_string();
    [[nodiscard]] unsigned long value_as_long() const noexcept;
    [[nodiscard]] ASTNode_wp value_as_node() const;

private:
    void accept(Visitor &v){
        v.visit(ASTNode_sp(this));
    }
}; // end class ASTNode

ASTNode_sp & attachChild(ASTNode_sp &&child, ASTNode_sp &parent);

} // end namespace elsix


