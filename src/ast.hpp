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

#include "visitor.hpp"
#include "tokens.hpp"

namespace elsix{


struct Location{
    Location() = default;
    explicit Location(int row, int column): row(row), column(column){}
    Location( const Location& other ) = default;

    Location& operator=( const Location& rhs ) = default;
    int row = -1;
    int column = -1;
};


struct LocationRangeImpl{
    LocationRangeImpl() = default;
    explicit LocationRangeImpl(const Location &start, const Location &end, int file_id)
        : start(start), end(end), file_id(file_id){}
    explicit LocationRangeImpl(const Location &start, int file_id)
        : start(start), end(Location()), file_id(file_id){}

    Location start;
    Location end;
    int file_id = -1;
};

// Convenience typedefs
using LocationRange = std::shared_ptr<LocationRangeImpl>;
class ASTNode;
// A parent owns its children.
using shared_ASTNode = std::shared_ptr<ASTNode>;
// A child does not own its parent.
using weak_ASTNode = std::weak_ptr<ASTNode>;

class ASTNode{
public:
    // Nodes must at least have a type (possibly undefined) and `LocationRange`.
    explicit ASTNode(NodeType t, const shared_ASTNode &parent, LocationRange location_range);
    explicit ASTNode(NodeType t, LocationRange location_range);
    ~ASTNode();

    int length();

    LocationRange location_range;

    /**
     * @brief What the node represents.
     */
    NodeType type;

    /**
     * @brief A parent owns its children.
     */
    std::vector<shared_ASTNode> children;

    /**
     * @brief A child does not own its parent.
     */
    weak_ASTNode parent;
    /**
     * @brief Holds a numeric value, a (pointer to) a string, or a pointer to a block.
     *
     * If the value is a string, we own the string and need to free it in dtor.
     */
    using ValueType = std::variant<unsigned long,
        const std::string *, weak_ASTNode >;
    ValueType value;

    const std::string *value_as_string() const;
    unsigned long value_as_long() const noexcept;
    weak_ASTNode value_as_node() const;

private:
    void accept(Visitor v){
        v.visit(std::shared_ptr<ASTNode>(this));
    }
}; // end class ASTNode

shared_ASTNode & attachChild(shared_ASTNode &&child, shared_ASTNode &parent);

} // end namespace elsix


