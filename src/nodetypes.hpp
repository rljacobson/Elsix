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

namespace elsix{

/**
 * @brief AST node types and token types.
 *
 * This enum class enumerates the AST node types as well as the token types generated by the
 * Tokenizer. L^6 is simple enough that it makes sense to conflate tokens/lexemes with AST nodes.
 * That is, we do not have separate token and AST node classes. Token objects are just leaf AST
 * node objects. The parser throws out whatever tokens do not belong in the AST after use.
 *
 * Note that the Tokenizer consumes some tokens without emitting an ASTNode. This handful of
 * tokens is not represented in NodeType. They appear as defines used by the Tokenizer only.
 * These tokens are:
 *      #define STAR_TOKEN '*'
 *      #define COMMENT_TOKEN ';'
 *      #define LPAREN_TOKEN '('
 *      #define RPAREN_TOKEN ')'
 *      #define COMMA_TOKEN ','
 *
 * NEWLINE does not appear explicitly in the code. By definition a NEWLINE satisfies
 * `isspace(c) && !isblank(c)`. End of line is signaled with `NodeType::EOL`, but not every
 * newline is a `NodeType::EOL`.
 */
enum class NodeType: uint_fast8_t{
    // Parser specific
        EOL,        // End of line.
    EOF_,        // End of file. (Needs `_` to avoid collision with EOF macro.)
    EMPTY,      // Token not found in a search.
    ERROR,      // Tokenization error.
    UNDEFINED,  // Token not yet determined.
    PROGRAM,
    
    // Branching, which we include as keywords
        IFANY,
    IF = IFANY,
    IFALL,
    IFNALL,
    IFNONE,
    NOT = IFNONE,
    THEN,
    
    // DO
        LINE,
    GOTO,
    DO = GOTO,
    DO_OR_FAIL,
    DO_STATE,
    DO_DUMP,
    DO_ADVANCE,
    DONE, // Keyword, return with success
    FAIL, // keyword, return with failure
    LABEL,
    
    // Labels/symbols
        ADVANC,
    STATE,
    DUMP,
    T_DOT, // `T.` The time in milliseconds since the program began.
    N_DOT, // `n.` The (potential) number of blocks available from the allocator, where n ranges
    // between 1 and 128.
    
    // Tests
        EQUALITY_TEST,
    INEQUALITY_TEST,
    GREATER_TEST,
    LESS_TEST,
    POINTS_SAME_BLOCK_TEST,
    ONE_BITS_OF_TEST,
    ZERO_BITS_OF_TEST,
    
    // Literals
        NUMBER_LITERAL,
    HOLLERITH_LITERAL,
    CONTENTS_LITERAL,  // Field name/bug/remote field.
    
    // Operations
    // Storage ops
        SETUP_STORAGE,
    DEFINE_FIELD,
    GET_BLOCK,
    FREE_BLOCK,
    SET_EQUAL,
    COPY_FIELD = SET_EQUAL,
    DUPLICATE_BLOCK,
    INTERCHANGE_CONTENTS,
    POINT_TO_SAME_AS,
    
    // Arithmetic
        ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    
    // Logical ops
        LOGICAL_OR,
    LOGICAL_AND,
    EXCLUSIVE_OR,
    COMPLEMENT,
    SHIFT_LEFT,
    SHIFT_RIGHT,
    RIGHT_ONES,
    RIGHT_ZEROES,
    LEFT_ONES,
    LEFT_ZEROES,
    COUNT_ONES,
    COUNT_ZEROES,
    
    // I/O
        INPUT,
    PRINT,
    PRINT_LIST,
    PUNCH,
    
    // Conversions - intended to facilitate I/O
        BINARY_TO_DECIMAL,
    DECIMAL_TO_BINARY,
    BINARY_TO_OCTAL,
    OCTAL_TO_BINARY,
    BLANKS_TO_ZEROES,
    ZEROES_TO_BLANKS,
    
    // Microfilm output
        TYPE_VERTICALLY,
    TYPE_HORIZONTALLY,
    X_RANGE,
    Y_RANGE,
    DRAW_LINE,
    DRAW_POINT,
    
    // Stack ops
        SAVE_FIELD_DEFINITION,
    RESTORE_FIELD_DEFINITION,
    SAVE_FIELD_CONTENTS,
    RESTORE_FIELD_CONTENTS,
    FIELD_CONTENTS_STACK,
    FIELD_DEFINITION_STACK,
    SAVE_TO_STACK,
    RETRIEVE_FROM_STACK,
    
    // Purely syntactic
        COMMA,
    LPAREN,
    RPAREN
};

enum class ArgType: uint_fast8_t{
    _,
    CD,
    C,
    D,
    O,
    H,
    CO,
    S,
    FIELD_NAME,
    DUMP_CONST,
    ZERO_CONST,
    ADVANC_CONST,
    STATE_CONST,
    R_CONST,
    S_CONST
};


// ToDo(Robert): Can we delete this?
/**
 * @brief Data types for operators. UNUSED.
 */
#define DECIMAL_OP_TYPE 'D'
#define OCTAL_OP_TYPE 'O'
#define HOLLERITH_OP_TYPE 'H'
#define CONTENTS_OP_TYPE 'C'


/**
 * @brief Other tokens for which no permanent AST node is necessary. A node object is created by the
 * tokenizer, returned to the parser, and then thrown away.
 *
 */
#define LPAREN_TOKEN '('
#define RPAREN_TOKEN ')'
#define COMMA_TOKEN ','
#define COMMENT_TOKEN ';' // No ASTNode or NodeType.

} // end namespace elsix
