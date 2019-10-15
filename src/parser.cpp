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

#include <iostream>
#include <string>
// #include <cstring>
#include <charconv>  // std::from_chars()

#include "fmt/format.h"

#include "parser.hpp"
#include "error.hpp"
#include "tokenstream.hpp"
#include "astnode.hpp"
#include "nodetypes.hpp"

namespace elsix{

// region: Parser constructors.

parser::parser(TokenStream &&token_stream):
    token_stream_(token_stream), error_handler_(std::make_unique<ErrorHandler>()){
}

parser::parser(TokenStream &&token_stream, ErrorHandler &&error_handler): token_stream_(token_stream){
    error_handler_ = std::make_unique<ErrorHandler>(error_handler);
}
// endregion: Parser constructors.

/**
 * @brief PROGRAM = LINE ('\n' LINE)*
 * @return The root node of the program.
 */
ASTNode_sp parser::parse(){
    ASTNode_sp root_node {
        std::make_shared<ASTNode>(NodeType::PROGRAM)
    };
    ASTNode_sp next_node{parse_line()};
    root_node->span.start = root_node->children.back()->span.end;

    while(next_node->type != NodeType::EOF_
            // ToDo: Should we stop on `NoteType::ERROR`? Perhaps distinguish error categories.
            && next_node->type != NodeType::ERROR){
        // Parse the line.
        attachChild(std::move(next_node), root_node);
        next_node = parse_line();
    }

    root_node->span.end = root_node->children.back()->span.end;
    return root_node;
}

/**
 * @brief line : LABEL? (ifexpr | thenexpr);
 * @return line node.
 */
ASTNode_sp parser::parse_line(){
    ASTNode *token{token_stream_.peek()};
    NodeType next_token_type = token->type;
    ASTNode_sp line_node{
        // All non leaf nodes initially have a zero length span, as they may not have any children.
        std::make_shared<ASTNode>(NodeType::LINE, token->span.start)
    };

    while(NodeType::EOL != next_token_type && NodeType::ERROR != next_token_type){
        // Alphabetic characters signal either a label or a keyword.
        switch(next_token_type){
        case NodeType::HOLLERITH_LITERAL: {
            ASTNode_sp child = attachChild(parse_label_or_keyword(), line_node);
            if(child->type == NodeType::LABEL){
                if(line_node->children.size() != 1){
                    // Line already has a label. Complain.
                    error_handler_->emitError(
                        fmt::format(
                            "Not a keyword, and a line can only have one label: {}",
                            *std::get<const std::string *>(child->value)), child->span
                    );
                } else{
                    // Labeled line. Record the label of this line node in the labels table.
                    labels_.insert(
                        std::pair<const std::string *, ASTNode_sp>(
                            // The label of the line.
                            std::get<const std::string *>(child->value),
                            // The line itself (as a shared_ptr).
                            line_node
                        ));
                } // End register labeled line.
            } // End label.
            break;
        }
        case NodeType::LPAREN: {
            // Must be operations in a THEN expression.
            attachChild(parse_then(), line_node);
            break;
        }
        default: {
                // Unexpected token.
                error_handler_->emitError(
                    fmt::format(
                        "Unexpected token: {}", token_stream_.span_to_string(token->span)
                    ), token->span
                );
            }
        } // End switch NodeType.
        token = token_stream_.peek();
    } // End while
    
    if(!line_node->children.empty()){
        line_node->span.end = line_node->children.end()->get()->span.end;
    }

    return line_node;
}


ASTNode_sp parser::parse_label_or_keyword(){
    ASTNode *next_token{token_stream_.peek()};

    // Determine if the token is a keyword or label.
    auto found = lookup_keyword(next_token->value_as_string());
    if(found == NodeType::EMPTY){
        // The token is a label.
        next_token->type = NodeType::LABEL;
    } else{
        // Keyword.
        next_token->type = found;
        switch(next_token->type){
            case NodeType::IFANY:
                [[fallthrough]];
            case NodeType::IFALL:
                [[fallthrough]];
            case NodeType::IFNALL:
                [[fallthrough]];
            case NodeType::IFNONE:
                return parse_if();
            case NodeType::THEN:
                return parse_then();
            case NodeType::DONE:
                return token_stream_.next();
            default:
            UNREACHABLE
        } // End switch on token type
    } // End keyword not found.

    return token_stream_.next();
}

/**
 * @brief Parses an IF-expression.
 *
 * An IF expression may contain a THEN delimiter, but it is not considered a
 * THEN expression. Rather, it is just part of the IF. If operationblock is
 * omitted, the goto is required.
 *
 * ifexpr : ifkeyword compoundcondition gotoexpr '\n'
 *        | ifkeyword compoundcondition THEN operationblock gotoexpr? '\n';
 *
 * @param if_expr
 */
ASTNode_sp parser::parse_if(){
    ASTNode_sp if_expr{std::make_shared<ASTNode>(NodeType::IF, token_stream_.startRange())};
    ASTNode *next_node = token_stream_.peek();

    // If must be followed by one or more test operations.
    while(next_node->type == NodeType::LPAREN){
        attachChild(parse_test(), if_expr);
        next_node = token_stream_.peek();
    }
    // Check for an empty compoundcondition, which is an error.
    if(if_expr->children.empty()){
        error_handler_->emitError("An IF statement cannot be empty.", if_expr->span);
    }

    if(next_node->type == NodeType::HOLLERITH_LITERAL){
        // Either a THEN or a GoTo, both of which are parsed by parse_then
        attachChild(parse_then(), if_expr);
    }

    // Record the end location of the statement.
    if_expr->span.end = token_stream_.here();

    return if_expr;

}

/**
 * @brief Parses a THEN expression.
 *
 * A THEN expression is a line that _begins_ with THEN, _not_ the end of an IF
 * expression. It is a standalone THEN.
 *
 * thenexpr : THEN? operationblock gotoexpr? '\n'
 *          | THEN gotoexpr '\n';
 *
 * @param then_expr: A THEN node or else an empty node.
 * @return
 */
ASTNode_sp parser::parse_then(){
    ASTNode *next_token{token_stream_.peek()};
    ASTNode_sp then_expr;

    if(next_token->type != NodeType::THEN){
        then_expr = std::make_shared<ASTNode>(NodeType::THEN, token_stream_.startRange());
    } else{
        // Consume the THEN token.
        then_expr = token_stream_.next();
    }

    // THEN may be followed by zero or more operations.
    while(next_token->type == NodeType::LPAREN){
        attachChild(parse_operation(), then_expr);
        next_token = token_stream_.peek();
    }

    // THEN or its operation list may be followed by a goto.
    if(next_token->type == NodeType::HOLLERITH_LITERAL){
        attachChild(parse_goto(token_stream_.next()), then_expr);
    }

    // Record the end location of the statement.
    then_expr->location_range->end = token_stream_.here();

    // Check for an empty THEN, which is an error. The goto is mandatory if
    // there are no operations.
    if(then_expr->children.empty()){
        error_handler_->emitError("A THEN statement cannot be empty.", then_expr->span);
    }

    return then_expr;
}


/**
 * @brief Parse a test, which is a boolean predicate of the form (c, op, cd).
 *
 * The second element in the list determines the test in every case.
 *
 * @return A node representing the test.
 */
ASTNode_sp parser::parse_test(){
    const std::string *op_code;
    OperationData *op_info;
    // There are exactly three terms to every test.
    ASTNode_sp arg1;
    ASTNode_sp op;
    ASTNode_sp arg2;
    // Consume '(', which should be the next character.
    expect(NodeType::LPAREN);

    arg1 = token_stream_.next();
    check_node_type(*arg1, NodeType::HOLLERITH_LITERAL);
    arg1->type = NodeType::CONTENTS_LITERAL;

    expect(NodeType::COMMA);

    op = token_stream_.next();
    arg1 = attachChild(std::move(arg1), op);
    check_node_type(*op, NodeType::HOLLERITH_LITERAL);
    op_info = lookup_op(op->value_as_string(), tests);
    if(nullptr == op_info){
        // Oops, not a test operator.
        error_handler_->emitError(
            fmt::format("Only tests are allowed here, but {} is not a test.", op->value_as_string()),
            op->span);
        // ToDo(Robert): Return from the function in a sensible way, making sure the next use of
        //  op_info is not on an uninitialized pointer.
    }
    op->type = op_info->type;

    expect(NodeType::COMMA);

    arg2 = token_stream_.next();
    arg2 = attachChild(std::move(arg2), op);

    // Parse the number literals.
    switch(op_info->arg_types[1]){
        case ArgType::D: //Decimal
            check_node_type(*arg2, NodeType::NUMBER_LITERAL);
            interpret_as_number(arg2);
            break;
        case ArgType::O:
            check_node_type(*arg2, NodeType::NUMBER_LITERAL);
            interpret_as_number(arg2, 8);
            break;
        case ArgType::C:
            arg2->type = NodeType::CONTENTS_LITERAL;
            break;
        case ArgType::CD:
            if(NodeType::NUMBER_LITERAL == arg2->type){
                interpret_as_number(arg2);
            } else{
                arg2->type = NodeType::CONTENTS_LITERAL;
            }
            break;
        case ArgType::CO:
            if(NodeType::NUMBER_LITERAL == arg2->type){
                interpret_as_number(arg2, 8);
            } else{
                arg2->type = NodeType::CONTENTS_LITERAL;
            }
            break;
        case ArgType::H:
            arg2->type = NodeType::NUMBER_LITERAL;
            error_handler_->emitError(
                "Hollerith literals are not implemented here.",
                op->span);
            // ToDo(Robert): Write a conversion function from "Hollerith" literals to numbers.
            //  The character set needs to be selectable.
            break;
        default:
            UNREACHABLE;
    }

    // Consume ')', which should be the next character.
    expect(NodeType::RPAREN);

    return op;
}

ASTNode_sp parser::parse_goto(ASTNode_sp label){
    check_node_type(*label, NodeType::HOLLERITH_LITERAL);
    label->type = NodeType::GOTO;
    label_gotos_.push_back(label);

    std::string_view text {label->value_as_string()};

    if("DUMP" == text){
        label->type = NodeType::DO_DUMP;
    } else if("STATE" == text){
        label->type = NodeType::DO_STATE;
    } else if("ADVANC" == text){
        label->type = NodeType::DO_ADVANCE;
    }
    return label;
}

/**
 * @brief Parse an operation, which has the form (c, op, cd).
 *
 * The second element in the list determines the operation in every case except for the DO
 * operator, and even in the case of DO, there is only one form, `(DO, s)`, that cannot be
 * determined from the second element.
 *
 * @return
 */
ASTNode_sp parser::parse_operation(){
    // const std::string_view op_code;
    // There can be up to 5 arguments.
    std::array<ASTNode_sp, 5> args;
    int argi = 0;
    ASTNode_sp delimiter;
    ASTNode_sp operation;

    // Consume '(', which should be the next character.
    expect(NodeType::LPAREN);

    // We gather up all the tokens first, because the number of expressions in the operation
    // helps determine which operation it is.
    for(argi = 0; argi < 5; argi++){
        args[argi] = token_stream_.next();
        delimiter = token_stream_.next();
        if(NodeType::RPAREN == delimiter->type){
            break;
        }
        check_node_type(*delimiter, NodeType::COMMA);
    }

    // We create a bespoke decision tree, since there are only 5 cases.
    switch(argi){
        // region: Special Cases

        case 0:
            // Empty or singleton operator.
            error_handler_->emitError(
                fmt::format("Operator cannot be empty: {}.",
                    delimiter->value_as_string()),
                delimiter->span);
            break;
        case 1:
            // Two arguments.
            // DO
            if(args[0]->type == NodeType::HOLLERITH_LITERAL
                && "DO" == args[0]->value_as_string()){
                return parse_goto(args[1]);
            } else{
                // There is only one other two-argument operator: the two argument form of (c, P, d).
                // In this abbreviated form, the second argument is a decimal literal.
                operation = std::make_shared<ASTNode>(
                    NodeType::POINT_TO_SAME_AS, args[0]->span
                );
                operation->location_range->end = token_stream_.here();
                args[0]->type = NodeType::CONTENTS_LITERAL;
                // The second argument should have already been identified as a number literal.
                check_node_type(*args[1], NodeType::NUMBER_LITERAL);
                interpret_as_number(args[1]);
                attachChild(std::move(args[0]), operation);
                attachChild(std::move(args[1]), operation);
            }
            break;
        case 2:{ // Scope of op_code
            // Three arguments.
            // The most common case. We handled the handful of special cases first.
            operation = args[1];
            
            const std::string_view op_code(operation->value_as_string());
        
            if("FR" == op_code){
                // Free block
                args[0]->type = NodeType::CONTENTS_LITERAL;
                operation->type = NodeType::FREE_BLOCK;
                if("0" == args[2]->value_as_string()){
                    args[2]->value = 0UL;
                    args[2]->type = NodeType::NUMBER_LITERAL;
                } else{
                    args[2]->type = NodeType::CONTENTS_LITERAL;
                }
                attachChild(std::move(args[0]), operation);
                attachChild(std::move(args[2]), operation);
            
            } else if("FC" == op_code){
                // Save/Restore Field Contents
                args[0]->type = NodeType::CONTENTS_LITERAL;
                if(args[0]->value_as_string() == "S"){
                    operation->type = NodeType::SAVE_FIELD_CONTENTS;
                
                } else if(args[0]->value_as_string() == "R"){
                    operation->type = NodeType::RESTORE_FIELD_CONTENTS;
                } else{
                    // Invalid option
                    error_handler_->emitError(
                        fmt::format(
                            "FC can only have S (save) or R (restore) but was given '{}'.",
                            args[0]->value_as_string()), args[0]->span
                    );
                }
                args[2]->type = NodeType::CONTENTS_LITERAL;
                attachChild(std::move(args[2]), operation);
                break;
            } else if("FD" == op_code){
                // Save/Restore Field Definition
                args[0]->type = NodeType::CONTENTS_LITERAL;
                if("S" == args[0]->value_as_string()){
                    operation->type = NodeType::SAVE_FIELD_DEFINITION;
                
                } else if(args[0]->value_as_string() == "R"){
                    operation->type = NodeType::RESTORE_FIELD_DEFINITION;
                } else{
                    // Invalid option
                    error_handler_->emitError(
                        fmt::format(
                            "FD can only have S (save) or R (restore) but was given '{}'.",
                            args[0]->value_as_string()), args[0]->span
                    );
                }
                args[2]->type = NodeType::CONTENTS_LITERAL;
                attachChild(std::move(args[2]), operation);
                break;
            }
        }
            // endregion: Special Cases
            // Let the generic case fall through
            [[fallthrough]];
        default:{  // Scope of op_code
            // Look for op in operators map.
            const std::string_view op_code(operation->value_as_string());
            auto op_info = lookup_op(op_code, operators);
            if(nullptr != op_info){
                operation = args[1];
                // ToDo: (Where I stopped.) Parse arguments according to `op_info` spec.
                
            } else{
                // Not in operators map. Must be a special case.
            }
        }

    }

    operation->location_range->end = token_stream_.here();
    return operation;
}

void parser::check_node_type(const ASTNode &node, NodeType expected){
    if(node.type != expected){
        error_handler_->emitError(
            fmt::format( "Expected {}, but got {}.", "expected", "node.type"),
            node.span
            );
    }
}

void parser::expect(NodeType expected){
    check_node_type(*token_stream_.peek(), expected);
}

/**
 * @brief Safe conversion of the value of the node from a pointer to a string representation of
 * a number in `base` to an unsigned long.
 *
 * All value manipulation should go through this function.
 *
 * @param node
 * @param type
 */
void parser::interpret_as_type(ASTNode_sp &node, ArgType type){
    switch(type){

        case ArgType::_:
            // Nothing to do.
            return;

        case ArgType::CD:
            if(NodeType::NUMBER_LITERAL == node->type){
                interpret_as_number(node);
            } else {
                node->type = NodeType::CONTENTS_LITERAL;
            }
            break;

        case ArgType::C:
            node->type = NodeType::CONTENTS_LITERAL;
            break;

        case ArgType::D:interpret_as_number(node);
            break;

        case ArgType::O:interpret_as_number(node, 8);
            break;

        case ArgType::H:
            // The node type *should* already be Hollerith, but just in case.
            node->type = NodeType::HOLLERITH_LITERAL;
            break;

        case ArgType::CO:
            if(NodeType::NUMBER_LITERAL == node->type){
                interpret_as_number(node, 8);
            } else {
                node->type = NodeType::CONTENTS_LITERAL;
            }
            break;

        case ArgType::S:
            // S is a location/label, not an S literal.
            node->type = NodeType::LABEL;
            // ToDo: Make sure the parent DO operation is added to the backpatch vector.
            break;

        case ArgType::FIELD_NAME:
            node->type = NodeType::HOLLERITH_LITERAL;
            break;

        case ArgType::ZERO_CONST:interpret_as_number(node);
            // ToDo: Should we emit an error if it's not zero?
            break;

        case ArgType::DUMP_CONST:
            // ToDo: Can we set the parent to DO_DUMP?
            node->type = NodeType::DO_DUMP;
            break;

        case ArgType::ADVANC_CONST:
            node->type = NodeType::DO_ADVANCE;
            break;

        case ArgType::STATE_CONST:
            node->type = NodeType::DO_DUMP;
            break;

        case ArgType::R_CONST:
            break;

        case ArgType::S_CONST:
            break;

        default:
            UNREACHABLE
    }

}

/**
 * @brief Safe conversion of the value of the node from a pointer to a string representation of
 * a decimal number to an unsigned long.
 *
 * @param node
 */
void parser::interpret_as_number(ASTNode_sp &node, int base){
    const std::string_view sv = node->value_as_string();
    unsigned long long_value = 0UL;

    // Attempt to convert the number.
    auto result = std::from_chars(sv.data(), sv.data() + sv.size(), long_value);
    
    if (result.ec == std::errc::invalid_argument) {
        error_handler_->emitError(
            fmt::format("Invalid argument: '{}'. Expected a literal number.", sv),
            node->span);
        node->type = NodeType::ERROR;
        return;
    }
    
    node->value = long_value;
    node->type = NodeType::NUMBER_LITERAL;
}


}
