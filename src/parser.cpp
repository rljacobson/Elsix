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
#include <cstring>
#include "fmt/format.h"

#include "parser.hpp"
#include "error.hpp"
#include "tokenizer.hpp"
#include "ast.hpp"

namespace elsix{

// region: Parser constructors.

parser::parser(Tokenizer &&token_stream):
    tokenstream_(token_stream), error_handler_(std::make_unique<ErrorHandler>()){
}

parser::parser(Tokenizer &&token_stream, ErrorHandler &&error_handler): tokenstream_(token_stream){
    error_handler_ = std::make_unique<ErrorHandler>(error_handler);
}
// endregion: Parser constructors.

/**
 * @brief PROGRAM = LINE ('\n' LINE)*
 * @return The root node of the program.
 */
shared_ASTNode parser::parse(){
    shared_ASTNode root_node {
        std::make_shared<ASTNode>(NodeType::PROGRAM, tokenstream_.startRange())
    };
    shared_ASTNode next_node{parseLine()};

    while(next_node->type != NodeType::DONE
            && next_node->type != NodeType::EMPTY
            && next_node->type != NodeType::ERROR){
        // Parse the line.
        attachChild(std::move(next_node), root_node);
        next_node = parseLine();
    }

    // ToDo(Robert): Handle error cases.

    root_node->location_range->end = tokenstream_.here();
    return root_node;
}

/**
 * @brief line : LABEL? (ifexpr | thenexpr);
 * @return line node.
 */
shared_ASTNode parser::parseLine(){
    shared_ASTNode line_node{
        std::make_shared<ASTNode>(NodeType::LINE, tokenstream_.startRange())
    };
    ASTNode &token{tokenstream_.peek()};
    NodeType next_token_type = tokenstream_.peek().type;

    while(NodeType::EMPTY != next_token_type && NodeType::ERROR != next_token_type){
        // Alphabetic characters signal either a label or a keyword.
        switch(next_token_type){
        case NodeType::HOLLERITH_LITERAL: {
            shared_ASTNode child = attachChild(parseLabelOrKeyword(), line_node);
            if(child->type == NodeType::LABEL){
                if(line_node->children.size() != 1){
                    // Line already has a label. Complain.
                    error_handler_->emitError(
                        fmt::format(
                            "Not a keyword, and a line can only have one label: {}",
                            *std::get<const std::string *>(child->value)), child->location_range
                    );
                } else{
                    // Labeled line. Record the label of this line node in the labels table.
                    labels.insert(
                        std::pair<const std::string *, shared_ASTNode>(
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
            attachChild(parseThen(), line_node);
            break;
        }
        default: {
                // Unexpected token.
                const std::string *error_token = tokenstream_.peekText();
                error_handler_->emitError(
                    fmt::format(
                        "Unexpected token: {}", std::string {*error_token}
                    ), line_node->location_range
                );
                delete error_token;
            }
        } // End switch NodeType.
        token = tokenstream_.peek();
    } // End while

    return line_node;
}


shared_ASTNode parser::parseLabelOrKeyword(){
    ASTNode &next_token{tokenstream_.peek()};

    // Determine if the token is a keyword or label.
    auto found = lookup_keyword(*next_token.value_as_string());
    if(found == NodeType::EMPTY){
        // The token is a label.
        next_token.type = NodeType::LABEL;
    } else{
        // Keyword.
        next_token.type = found;
        switch(next_token.type){
            case NodeType::IFANY:
                [[fallthrough]];
            case NodeType::IFALL:
                [[fallthrough]];
            case NodeType::IFNALL:
                [[fallthrough]];
            case NodeType::IFNONE:
                return parseIf();
            case NodeType::THEN:
                return parseThen();
            case NodeType::DONE:
                return tokenstream_.next();
            default:
            UNREACHABLE
        } // End switch on token type
    } // End keyword not found.

    return tokenstream_.next();
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
shared_ASTNode parser::parseIf(){
    shared_ASTNode if_expr{std::make_shared<ASTNode>(NodeType::IF, tokenstream_.startRange())};
    ASTNode &next_node = tokenstream_.peek();

    // If must be followed by one or more test operations.
    while(next_node.type == NodeType::LPAREN){
        attachChild(parseTest(), if_expr);
        next_node = tokenstream_.peek();
    }
    // Check for an empty compoundcondition, which is an error.
    if(if_expr->children.empty()){
        error_handler_->emitError("An IF statement cannot be empty.", if_expr->location_range);
    }

    if(next_node.type == NodeType::HOLLERITH_LITERAL){
        // Either a THEN or a GoTo, both of which are parsed by parseThen
        attachChild(parseThen(), if_expr);
    }

    // Record the end location of the statement.
    if_expr->location_range->end = tokenstream_.here();

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
shared_ASTNode parser::parseThen(){
    ASTNode &next_token{tokenstream_.peek()};
    shared_ASTNode then_expr;

    if(next_token.type != NodeType::THEN){
        then_expr = std::make_shared<ASTNode>(NodeType::THEN, tokenstream_.startRange());
    } else{
        // Consume the THEN token.
        then_expr = tokenstream_.next();
    }

    // THEN may be followed by zero or more operations.
    while(next_token.type == NodeType::LPAREN){
        attachChild(parseOperation(), then_expr);
        next_token = tokenstream_.peek();
    }

    // THEN or its operation list may be followed by a goto.
    if(next_token.type == NodeType::HOLLERITH_LITERAL){
        attachChild(parseGoTo(tokenstream_.next()), then_expr);
    }

    // Record the end location of the statement.
    then_expr->location_range->end = tokenstream_.here();

    // Check for an empty THEN, which is an error. The goto is mandatory if
    // there are no operations.
    if(then_expr->children.empty()){
        error_handler_->emitError("A THEN statement cannot be empty.", then_expr->location_range);
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
shared_ASTNode parser::parseTest(){
    const std::string *op_code;
    OperationData *op_info;
    // There are exactly three terms to every test.
    shared_ASTNode arg1, op, arg2;
    // Consume '(', which should be the next character.
    expect(NodeType::LPAREN);

    arg1 = tokenstream_.next();
    checkNodeType(*arg1, NodeType::HOLLERITH_LITERAL);
    arg1->type = NodeType::CONTENTS_LITERAL;

    expect(NodeType::COMMA);

    op = tokenstream_.next();
    arg1 = attachChild(std::move(arg1), op);
    checkNodeType(*op, NodeType::HOLLERITH_LITERAL);
    op_info = lookup_op(*op->value_as_string(), tests);
    if(nullptr == op_info){
        // Oops, not a test operator.
        error_handler_->emitError(
            fmt::format("Only tests are allowed here, but {} is not a test.", op->value_as_string()),
            op->location_range);
        // ToDo(Robert): Return from the function in a sensible way.
    }
    op->type = op_info->type;

    expect(NodeType::COMMA);

    arg2 = tokenstream_.next();
    arg2 = attachChild(std::move(arg2), op);

    // Parse the number literals.
    switch(op_info->arg_types[1]){
        case ArgType::D: //Decimal
            checkNodeType(*arg2, NodeType::NUMBER_LITERAL);
            arg2->value = std::stoul(*arg2->value_as_string());
            break;
        case ArgType::O:
            checkNodeType(*arg2, NodeType::NUMBER_LITERAL);
            arg2->value = std::stoul(*arg2->value_as_string(), nullptr, 8);
            break;
        case ArgType::C:
            arg2->type = NodeType::CONTENTS_LITERAL;
            break;
        case ArgType::CD:
            if(NodeType::NUMBER_LITERAL == arg2->type){
                arg2->value = std::stoul(*arg2->value_as_string());
            } else{
                arg2->type = NodeType::CONTENTS_LITERAL;
            }
            break;
        case ArgType::CO:
            if(NodeType::NUMBER_LITERAL == arg2->type){
                arg2->value = std::stoul(*arg2->value_as_string(), nullptr, 8);
            } else{
                arg2->type = NodeType::CONTENTS_LITERAL;
            }
            break;
        case ArgType::H:
            arg2->type = NodeType::NUMBER_LITERAL;
            error_handler_->emitError(
                "Hollerith literals are not implemented here.",
                op->location_range);
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

shared_ASTNode parser::parseGoTo(shared_ASTNode label){
    checkNodeType(*label, NodeType::HOLLERITH_LITERAL);
    label->type = NodeType::GOTO;
    label_gotos.push_back(label);

    std::string text {*label->value_as_string()};

    if(DUMP_TOKEN == text){
        label->type = NodeType::DO_DUMP;
    } else if(STATE_TOKEN == text){
        label->type = NodeType::DO_STATE;
    } else if(ADVANCE_TOKEN == text){
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
shared_ASTNode parser::parseOperation(){
    const std::string *op_code;
    // There can be up to 5 arguments.
    std::array<shared_ASTNode, 5> args;
    int argi = 0;
    shared_ASTNode delimiter;
    shared_ASTNode operation;

    // Consume '(', which should be the next character.
    expect(NodeType::LPAREN);

    // We gather up all the tokens first, because the number of expressions in the operation
    // helps determine which operation it is.
    for(argi = 0; argi < 5; argi++){
        args[argi] = tokenstream_.next();
        delimiter = tokenstream_.next();
        if(NodeType::RPAREN == delimiter->type){
            break;
        }
        checkNodeType(*delimiter, NodeType::COMMA);
    }

    // We create a bespoke decision tree, since there are only 5 cases.
    switch(argi){
        // region: Special Cases

        case 0:
            // Empty or singleton operator.
            error_handler_->emitError(
                fmt::format("Operator cannot be empty: {}.",
                    delimiter->value_as_string()),
                delimiter->location_range);
            break;
        case 1:
            // Two arguments.
            // DO
            if(args[0]->type == NodeType::HOLLERITH_LITERAL
                && DO_TOKEN == *args[0]->value_as_string()){
                return parseGoTo(args[1]);
            } else{
                // There is only one other two-argument operator: the two argument form of (c, P, d).
                // In this abbreviated form, the second argument is a decimal literal.
                operation = std::make_shared<ASTNode>(
                    NodeType::POINT_TO_SAME_AS, args[0]->location_range
                );
                operation->location_range->end = tokenstream_.here();
                args[0]->type = NodeType::CONTENTS_LITERAL;
                // The second argument should have already been identified as a numnber literal.
                checkNodeType(*args[1], NodeType::NUMBER_LITERAL);
                // args[1]->type = NodeType::NUMBER_LITERAL;
                args[1]->value = std::stoul(*args[1]->value_as_string());
                attachChild(std::move(args[0]), operation);
                attachChild(std::move(args[1]), operation);
            }
            break;
        case 2:
            // Three arguments.
            // The most common case. We handled the handful of special cases first.
            operation = args[1];
            op_code = operation->value_as_string();

            if("FR" == *op_code){
                // Free block
                args[0]->type = NodeType::CONTENTS_LITERAL;
                operation->type = NodeType::FREE_BLOCK;
                if(*args[2]->value_as_string() == "0"){
                    args[2]->value = 0ul;
                    args[2]->type = NodeType::NUMBER_LITERAL;
                } else{
                    args[2]->type = NodeType::CONTENTS_LITERAL;
                }
                attachChild(std::move(args[0]), operation);
                attachChild(std::move(args[2]), operation);

            } else if("FC" == *op_code){
                // Save/Restore Field Contents
                args[0]->type = NodeType::CONTENTS_LITERAL;
                if(*args[0]->value_as_string() == "S"){
                    operation->type = NodeType::SAVE_FIELD_CONTENTS;

                } else if(*args[0]->value_as_string() == "R"){
                    operation->type = NodeType::RESTORE_FIELD_CONTENTS;
                } else {
                    // Invalid option
                    error_handler_->emitError(
                        fmt::format("FC can only have S (save) or R (restore) but was given '{}'.",
                                    args[0]->value_as_string()),
                        args[0]->location_range);
                }
                args[2]->type = NodeType::CONTENTS_LITERAL;
                attachChild(std::move(args[2]), operation);
                break;
            } else if("FD" == *op_code){
                // Save/Restore Field Definition
                args[0]->type = NodeType::CONTENTS_LITERAL;
                if(*args[0]->value_as_string() == "S"){
                    operation->type = NodeType::SAVE_FIELD_DEFINITION;

                } else if(*args[0]->value_as_string() == "R"){
                    operation->type = NodeType::RESTORE_FIELD_DEFINITION;
                } else {
                    // Invalid option
                    error_handler_->emitError(
                        fmt::format("FD can only have S (save) or R (restore) but was given '{}'.",
                                    args[0]->value_as_string()),
                        args[0]->location_range);
                }
                args[2]->type = NodeType::CONTENTS_LITERAL;
                attachChild(std::move(args[2]), operation);
                break;
            }
            // endregion: Special Cases
            // Let the generic case fall through
            [[fallthrough]];
        default:
            // Look for op in operators map.
            auto op_info = lookup_op(*op_code, operators);
            if(nullptr != op_info) {
                operation = args[1];
                // ToDo: Wherer I stopped.

                // ToDo: Refactor everything using a number so that it goes through
                //  `interpretAsNumber`.


            } else{
                // Not in operators map. Must be a special case.
            }


    }

    operation->location_range->end = tokenstream_.here();
    return operation;
}

void parser::checkNodeType(const ASTNode &node, NodeType expected){
    if(node.type != expected){
        error_handler_->emitError(
            fmt::format( "Expected {}, but got {}.", "expected", "node.type"),
            node.location_range
            );
    }
}

void parser::expect(NodeType expected){
    checkNodeType(tokenstream_.peek(), expected);

}

/**
 * @brief Safe converrsion of the value of the node from a pointer to a string representation of
 * a number in `base` to an unsigned long.
 *
 * All value manipulation should go through this function.
 *
 * @param node
 * @param type
 */
void parser::interpretAsType(shared_ASTNode &node, ArgType type){
    switch(type){

        case ArgType::_:
            // Nothing to do.
            return;

        case ArgType::CD:
            if(NodeType::NUMBER_LITERAL == node->type){
                interpretAsNumber(node);
            } else {
                node->type = NodeType::CONTENTS_LITERAL;
            }
            break;

        case ArgType::C:
            node->type = NodeType::CONTENTS_LITERAL;
            break;

        case ArgType::D:
            interpretAsNumber(node);
            break;

        case ArgType::O:
            interpretAsNumber(node, 8);
            break;

        case ArgType::H:
            // The node type *should* already be Hollerith, but just in case.
            node->type = NodeType::HOLLERITH_LITERAL;
            break;

        case ArgType::CO:
            if(NodeType::NUMBER_LITERAL == node->type){
                interpretAsNumber(node, 8);
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

        case ArgType::ZERO_CONST:
            interpretAsNumber(node);
            // ToDo: Should we emit an error if it's not zero?
            break;

        case ArgType::DUMP_CONST:
            // ToDo: Can we set the parent to DO_DUMP?
            node->type = NodeType::DO_DUMP;
            delete node->value_as_string();
            break;

        case ArgType::ADVANC_CONST:
            node->type = NodeType::DO_ADVANCE;
            delete node->value_as_string();
            break;

        case ArgType::STATE_CONST:
            node->type = NodeType::DO_DUMP;
            delete node->value_as_string();
            break;

        case ArgType::R_CONST:
            delete node->value_as_string();
            break;

        case ArgType::S_CONST:
            delete node->value_as_string();
            break;

        default:
            UNREACHABLE
    }

}

/**
 * @brief Safe converrsion of the value of the node from a pointer to a string representation of
 * a decimal number to an unsigned long.
 *
 * @param node
 */
void parser::interpretAsNumber(shared_ASTNode &node, int base){
    const std::string *reclaim = node->value_as_string();

    // Attempt to convert the number.
    try {
        node->value = std::stoul(*reclaim, nullptr, base);
    }
    catch (const std::invalid_argument& ia) {
        error_handler_->emitError(
            fmt::forrmat("Invalid argument: '{}'. Expected a decimal =literral.", reclaim),
            node->location_range);
        // Does not free node->value string.
        node->type = NodeType::ERROR;
        return;
    }

    delete reclaim;
    node->type = NodeType::NUMBER_LITERAL;
}


}
