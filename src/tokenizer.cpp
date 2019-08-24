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

#include <sstream>

#include "string_utilities.hpp"
#include "error.hpp"
#include "ast.hpp"
#include "tokenizer.hpp"

namespace elsix{

// ToDo: This tokenizer is poorly engineered. There are 10 functions implemented below that all
//  have names that are essentially synonyms for "supply the next piece of data." The memory
//  management is ad hoc, and the scanner's interaction with the error reporting system is obscured.
//  Refactor into a zero copy scanner.

// region: Tokenizer ctors

Tokenizer::Tokenizer(std::istream &in):
    in_(in), row_(0), column_(0), file_id_(0), current_token_(nullptr),
    error_handler_ (*(new ErrorHandler)){
    // ToDo(Robert): Is LINE_MAX in the C++ standard? It appears to be POSIX.
    // LINE_MAX is the maximum number of bytes that can be read by getline().
    current_line_.reserve(LINE_MAX);
}

Tokenizer::Tokenizer(std::istream &in, ErrorHandler &&error_handler):
    in_(in), row_(0), column_(0), file_id_(0), current_token_(nullptr),
    error_handler_(error_handler){
}

Tokenizer::~Tokenizer(){
    delete &error_handler_;
    delete current_token_;
}
// endregion: Tokenizer ctors
// region: Location related functions.

/**
 * @brief A factory function that creates a LocationRange initialized with start = here().
 * @return LocationRange object
 */
LocationRange Tokenizer::startRange() const {
    // LocationRange is a shared_ptr to LocationRangeImpl
    return std::make_shared<LocationRangeImpl>(here(), file_id_);
}

/**
 * @brief A factory function that creates a LocationRange initialized with start = s and end =
 * here().
 *
 * @return LocationRange object
 */
LocationRange Tokenizer::endRange(Location start) const {
    // LocationRange is a shared_ptr to LocationRangeImpl
    return std::make_shared<LocationRangeImpl>(start, here(), file_id_);
}

/**
 * @brief The current cursor location in the input as a row/column pair.
 * @return A location object value, a struct containing a row/column pair.
 */
Location Tokenizer::here() const noexcept{
    return Location(row_, column_);
}
// endregion: Location related functions
// region: Character stream peek and next functions.

/**
 * @brief Consumes a single character of the input, returning its value.
 * @return The char value.
 */
char Tokenizer::nextChar_(){
    // ToDo(Robert): This method is dumb. We should read a line at a time into `current_line_` so
    //  that we can give the entire line to the error handler. Then we should read a character at
    //  a time from current_line_.

    // If we attempt to continue reading past EOF, return 0.
    if(in_.eof()){
        return 0;
    }

    char next = in_.get();

    if(in_.eof()){
        return 0;
    }else if(in_.fail()){
        error_handler_.emitFatalError("Unable to read from input stream.", endRange(here()));
        return 0;
    }

    // Eat comments
    if(COMMENT_TOKEN == next){
        do{
            next = in_.get();
            column_++;
        } while(next && !isNewline(next));
    }

    if(isNewline(next)){
        ++row_;
        // Columns are 1-based in that Column 0 is the newline.
        column_ = 0;
    }else{
        // We clear the line on first character of new line to allow any generated errors to use
        // the line after a NEWLINE is encountered.
        if(0 == column_){
            current_line_.clear();
        }
        // Append character to current line.
        current_line_.push_back(next);
        column_++;
    }
    return next;
}

/**
 * @brief Consumes all blanks and the first single nonblank character
 *        of the input, returning its value.
 *
 * @return The char value.
 */
char Tokenizer::nextNonBlank_(){
    char in_char;

    do{
        in_char = nextChar_();
    } while(isBlank(in_char));

    return in_char;
}

/**
 * @brief Returns the next character in the stream without consuming it.
 * @return The char value.
 */
char Tokenizer::peekChar_(){
    return in_.peek();
}
// endregion: Character stream peek and next functions.
// region: Token stream expect, peek, and next functions.

/**
 * @brief Consumes characters of character stream, storing the next token in current_token and
 * setting the `token_type` to its best guess.
 *
 */
void Tokenizer::getPeekToken_(){
    // The `current_token_` is always owned by a `shared_ptr` that is returned by `next_char()`,
    // which then sets `current_token_` to `nullptr`. Note: We assume here that `getPeekToken_()`
    // is not called unless `expect()` or `next_char()` is called first, clearing `current_token_`.
    // Warning: Nothing enforces this contract!
    assert(nullptr == current_token_);
    current_token_ = new ASTNode(NodeType::UNDEFINED, startRange());
    char next_char = nextNonBlank_();
    std::string text{next_char};
    bool is_number = isDigit(next_char);
    bool is_hollerith = isHollerith(next_char);
    bool is_newline = isNewline(next_char);

    // The first `next_char` character is special, because it is `nextNonBlank_`,
    // i.e. it does not have to be contiguous with the previously read character.
    if(is_number || is_hollerith){
        while(isAlphanumeric(next_char = peekChar_())){
            is_number = is_number && isDigit(next_char);
            is_hollerith = is_hollerith && isHollerith(next_char);
            is_newline = is_newline && isNewline(next_char);
            text.push_back(nextChar_());
        }
    }

    current_token_->location_range->end = here();

    // We do some initial rudimentary NodeType guessing. The parser knows more about how to
    // determine NodeType.
    if(is_number){
        current_token_->type = NodeType::NUMBER_LITERAL;
        // We do not know yet which base the number is in.
        current_token_->value = new std::string{text};
        return;
    } else if(is_hollerith){
        current_token_->type = NodeType::HOLLERITH_LITERAL;
        current_token_->value = new std::string{text};
        return;
    } else if(is_newline){
        current_token_->type = NodeType::EMPTY;
        return;
    }
    // The remaining checks only apply to single character tokens.
    if(current_token_->length() != 1){
        return;
    }
    switch(current_line_[ current_token_->location_range->end.row - 1 ]){
        case COMMA_TOKEN:
            current_token_->type = NodeType::COMMA;
            break;
        case LPAREN_TOKEN:
            current_token_->type = NodeType::LPAREN;
            break;
        case RPAREN_TOKEN:
            current_token_->type = NodeType::RPAREN;
            break;
    } // End switch on single character
    // Default type is UNDEFINED.
}


/**
 * @brief Returns but does not consume the next token in the token stream.
 *
 * @return The next token.
 */
ASTNode &Tokenizer::peek(){
    if(current_token_ != nullptr){
        return *current_token_;
    }
    getPeekToken_();
    // const ASTNode &token{*current_token_};
    return *current_token_;
}

/**
 * @brief Returns the text of the current token or the empty string if there is no current token.
 *
 * @return The text of the current token.
 */
const std::string *Tokenizer::peekText() const noexcept {
    if(current_token_ != nullptr){
        return new std::string{
            current_line_.substr(
            current_token_->location_range->start.column - 1,
            current_token_->length()
            )
        };
    }

    return new std::string{""};
}

/**
 * @brief Checks if the next token if the string given by `expected` and,
 *      if it is, consumes the token and returns `true`. Otherwise, it does
 *      not consume the token and returns false.
 * @param expected : The string that is expected next in the token stream.
 * @return Boolean, whether the next token matches expected.
 */
bool Tokenizer::expect(const std::string &expected){
    if(nullptr == current_token_){
        getPeekToken_();
    }
    int pos = current_token_->location_range->start.column - 1;
    int count = current_token_->location_range->end.column - pos;
    // `string.compare()` returns 0 if the substring matches the supplied string.
    if(0 == current_line_.compare(pos, count, expected)){
        // This is the only time it makes sense for us to free `current_token_`.
        if(nullptr != current_token_){
            delete current_token_;
            current_token_ = nullptr;
        }
        return true;
    }
    return false;
}

/**
 * @brief A convenient specialization of expect for the simpler case that we expect a char.
 * @param expected
 * @return Boolean, whether expected is the next character in the char stream.
 */
bool Tokenizer::expect(char expected){
    if(nullptr == current_token_){
        getPeekToken_();
    }
    if(1 == current_token_->length() && current_line_.back() == expected){
        delete current_token_;
        current_token_ = nullptr;
        return true;
    }
    return false;
}

/**
 * @brief A convenient specialization of expect for the simpler case that we expect a char.
 * @param expected
 * @return Boolean, whether expected is the next character in the char stream.
 */
bool Tokenizer::expect(NodeType expected){
    if(nullptr == current_token_){
        getPeekToken_();
    }
    if(expected == current_token_->type){
        delete current_token_;
        current_token_ = nullptr;
        return true;
    }
    return false;
}


shared_ASTNode Tokenizer::next(){
    if(nullptr==current_token_){
        getPeekToken_();
    }
    shared_ASTNode token = std::make_shared<ASTNode>(std::move(*current_token_));
    // The current_token_ is freed by the shared_ptr.
    current_token_ = nullptr;
    return token;
}
// endregion: expect, peek, and next

}
