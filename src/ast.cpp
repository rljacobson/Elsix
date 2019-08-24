/*
    Created by Robert Jacobson on 04 April 2019.
    
    capitalize(${PROJECT_NAME})
    Description:  
    
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

#include "ast.hpp"

namespace elsix{

ASTNode::ASTNode(NodeType t, const shared_ASTNode &parent, LocationRange location_range):
    location_range(std::move(location_range)),
    type(t),
    parent(parent),
    value{0ul}{
}

int ASTNode::length(){
    int pos = location_range->start.column - 1;
    return location_range->end.column - pos;
}


ASTNode::ASTNode(NodeType node_type, LocationRange location_range):
    location_range(std::move(location_range)),
    type(node_type),
    value{0ul}{
}

ASTNode::~ASTNode(){
    // If we own a string on the heap, we need to delete it.
    if(NodeType::HOLLERITH_LITERAL == type
        && nullptr != std::get<const std::string *>(value)){
        delete std::get<const std::string *>(value);
    }
}

inline const std::string *ASTNode::value_as_string() const{
    return std::get<const std::string *>(value);
}
inline unsigned long ASTNode::value_as_long() const noexcept{
    return std::get<unsigned long>(value);
}
weak_ASTNode ASTNode::value_as_node() const{
    return std::get<weak_ASTNode>(value);
}

shared_ASTNode & attachChild(shared_ASTNode &&child, shared_ASTNode &parent){
    child->parent = std::weak_ptr<ASTNode>{parent};
    parent->children.push_back(std::move(child));
    return parent->children.back();
}

}
