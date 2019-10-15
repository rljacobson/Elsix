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

#include "astnode.hpp"
#include "nodetypes.hpp"

namespace elsix{

ASTNode::ASTNode():
    span(Span()),
    type(NodeType::UNDEFINED),
    value(0UL){
}

ASTNode::ASTNode(NodeType t):
    span(Span()),
    type(t),
    value(0UL){
}

ASTNode::ASTNode(NodeType t, Location loc):
    span(Span(loc, loc)),
    type(t),
    value(0UL){
}

ASTNode::ASTNode(NodeType t, Span s):
    span(s),
    type(t),
    value(0UL){
}

ASTNode::~ASTNode(){
    // If we own a string on the heap, we need to delete it.
    if(NodeType::HOLLERITH_LITERAL == type
        && nullptr != std::get<const std::string_view>(value)){
        delete std::get<const std::string_view>(value);
    }
}

inline const std::string_view ASTNode::value_as_string(){
    return std::get<const std::string_view>(value);
}
inline unsigned long ASTNode::value_as_long() const noexcept{
    return std::get<unsigned long>(value);
}
ASTNode_wp ASTNode::value_as_node() const{
    return std::get<ASTNode_wp>(value);
}

ASTNode_sp & attachChild(ASTNode_sp &&child, ASTNode_sp &parent){
    child->parent = std::weak_ptr<ASTNode>{parent};
    parent->children.push_back(std::move(child));
    return parent->children.back();
}

}
