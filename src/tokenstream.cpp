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

#include "stringutilities.hpp"
#include "tokenstream.hpp"
#include "sourcefile.hpp"
#include "nodetypes.hpp"
#include "error.hpp"
#include "astnode.hpp"
#include "location.hpp"

namespace elsix{

TokenStream::TokenStream(const std::string &source_filename) : source_file_(
    SourceFile(
        source_filename
    )), error_handler_(ErrorHandler()){
    cursor_ = Location(0, -1);
}

ASTNode_sp TokenStream::next(){
    // If a token has not been staged, stage it.
    if(nullptr == staged_token_){
        tokenize_next_();
    }
    ASTNode_sp token = std::make_shared<ASTNode>(std::move(*staged_token_));
    // The staged_token_ is freed by the shared_ptr.
    staged_token_ = nullptr;
    return token;
}

/**
 * @brief Returns the next character in the stream and consumes it.
 * @return The next character in the character stream.
 */
char TokenStream::next_char_(){
    // If we are already at EOF, return `EOF_CHARACTER`.
    if(cursor_ == source_file_.eof()){
        return EOF_CHARACTER;
    }
    
    // Otherwise, move cursor_ to the next position.
    if(cursor_.column == static_cast<int>(source_file_.lines[cursor_.row].length() - 1)){
        // We're at EOL. The next position is on the next line.
        cursor_.row++;
        cursor_.column = 0;
        return EOL_CHARACTER;
    } else{
        cursor_.column++;
    }
    
    // If we're at EOF, return `EOF_CHARACTER`.
    if(cursor_ == source_file_.eof()){
        return EOF_CHARACTER;
    }
    
    return source_file_.lines[cursor_.row][cursor_.column];
}

/**
 * @brief Returns the next character in the stream without consuming it.
 * @return The next character in the character stream.
 */
char TokenStream::peek_char_(){
    // This code is identical to `next_char_()` except that it uses a copy of `cursor_`
    // instead of `cursor_` itself. A bit non-DRY.
    
    // If we are already at EOF, return `EOF_CHARACTER`.
    if(cursor_ == source_file_.eof()){
        return EOF_CHARACTER;
    }
    
    Location next_location = cursor_;
    
    // Otherwise, move cursor_ to the next position.
    if(next_location.column == static_cast<int>(source_file_.lines[cursor_.row].length() - 1)){
        // We're at EOL. The next position is on the next line.
        next_location.row++;
        next_location.column = 0;
        return EOL_CHARACTER;
    } else{
        next_location.column++;
    }
    
    // If we're at EOF, return `EOF_CHARACTER`.
    if(next_location == source_file_.eof()){
        return EOF_CHARACTER;
    }
    
    return source_file_.lines[next_location.row][next_location.column];
}

void TokenStream::tokenize_next_(){
    // The `staged_token_` is always owned by a `shared_ptr` that is returned by `next()`,
    // which then sets `staged_token_` to `nullptr`. Note: We assume here that `tokenize_next_()`
    // is not called unless `expect()` or `next()` is called first, clearing `staged_token_`.
    // Warning: Nothing enforces this contract but this assert!
    assert(nullptr == staged_token_);
    
    staged_token_ = new ASTNode(NodeType::UNDEFINED, Span(cursor_));
    char c = next_nonblank_char_();
    
    // Since we are scanning the token anyway, we might as well estimate its type.
    bool is_number = isDigit(c);             // Does not detect hex.
    bool is_hollerith = isHollerith(c);      // Hollerith literals are alphanumeric or '.'.
    bool is_newline = (EOL_CHARACTER == c);  // Newlines are normalized to EOL_CHARACTER by
    // next_char_().
    
    // The first `c` character is special, because it is `nextNonBlank_`,
    // i.e. it does not have to be contiguous with the previously read character.
    if(is_number || is_hollerith){
        while(isAlphanumeric(c = peek_char_())){
            // ToDo: If it starts with a number, it must be a number. Otherwise, emit an error.
            is_number = is_number && isDigit(c);
            is_hollerith = is_hollerith && isHollerith(c);
            next_char_();
        }
    } else if(is_newline){
        // We don't bother reporting multiple consecutive newlines. However, adjacent newlines
        // are included in the span.
        while(EOL_CHARACTER == (c = peek_char_())){
            next_char_();
        }
    }
    
    // Adjust `end` to be one past the last char.
    staged_token_->span.end = cursor_;
    staged_token_->span.end.column++;
    
    // We do some initial rudimentary NodeType guessing. The parser knows more about how to
    // determine NodeType, because it has more context, so this is just a hint to the parser. For
    // example, `deadbeef` might be a hex number literal.
    if(is_number){
        staged_token_->type = NodeType::NUMBER_LITERAL;
        staged_token_->value = span_to_string(staged_token_->span);
        // We do not know yet which base the number is in.
        return;
    } else if(is_hollerith){
        staged_token_->type = NodeType::HOLLERITH_LITERAL;
        staged_token_->value = span_to_string(staged_token_->span);
        return;
    } else if(is_newline){
        staged_token_->type = NodeType::EOL;
        return;
    }
    // The remaining checks only apply to single character tokens.
    if(source_file_.span_length(staged_token_->span) != 1){
        return;
    }
    switch(*source_file_.loc_to_ptr(staged_token_->span.start)){
        // ToDo: Figure out how `T.` and `n.` were intended to work, and implement them here.
        case EOF_CHARACTER:staged_token_->type = NodeType::EOF_;
            break;
        case COMMA_TOKEN:staged_token_->type = NodeType::COMMA;
            break;
        case LPAREN_TOKEN:staged_token_->type = NodeType::LPAREN;
            break;
        case RPAREN_TOKEN:staged_token_->type = NodeType::RPAREN;
            break;
    } // End switch on single character
    
    // Default NodeType is UNDEFINED.
}

/**
 * @brief Consumes all blanks and the first single nonblank character
 *        of the input, returning its value.
 *
 * @return The char value.
 */
char TokenStream::next_nonblank_char_(){
    char c = next_char_();
    
    while(isBlank(c)){
        c = next_char_();
    }
    
    // Eat comments
    if(COMMENT_TOKEN == c){
        do{
            c = next_char_();
        } while(c != EOF_CHARACTER && c != EOL_CHARACTER);
    }
    
    return c;
}

/**
 * @brief Returns a non-owning reference to the next token but retains ownership until `next()`
 * is called.
 *
 * @return An `ASTNode&`.
 */
ASTNode *TokenStream::peek(){
    if(staged_token_ != nullptr){
        return staged_token_;
    }
    tokenize_next_();
    return staged_token_;
}

/**
 * @brief Exposes `SourceFile.span_to_string(Span span)`.
 * @param span: A span within the file.
 * @return: A `string_view` containing the text covered by the span.
 */
std::string_view TokenStream::span_to_string(Span span) const{
    return source_file_.span_to_string(span);
}

}

