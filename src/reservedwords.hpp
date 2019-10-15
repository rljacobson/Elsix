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

/**
 * @brief A database of information about keywords and function signatures.
 */


#include <unordered_map>
#include <array>
#include <vector>
#include <string>
#include "nodetypes.hpp"

namespace elsix{

/**
 * @brief We use a std::unordered_map to create a database of information about each operator.
 * The rows of the database are instances of `OperationData`. The keys are the `name`.
 */
struct OperationData{
    const NodeType type;
    // ToDo: This is gratuitous.
    const char name[4]; // NOLINT(hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    const std::array<ArgType, 4> arg_types;
    const std::string description;
};

// const OperationData thing {
//     NodeType::COMMA,
//     "hi\0",
//     {ArgType::_, ArgType::_, ArgType::_, ArgType::_},
//     "A thing to test the stuff"
// };
using OperatorMap  = std::unordered_map<char[4], OperationData>; // NOLINT(hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
using KeywordMap  = std::unordered_map<std::string_view, NodeType>;

/**
 * @brief Keyword tokens.
 *
 * Here we use the word keyword to mean a reserved word that can begin a statement. There are
 * other operators and reserved words that are not keywords.
 */
extern KeywordMap keywords;
NodeType lookup_keyword(const std::string_view key);

/**
 * @brief Database of operator information, where operator `name` is the lookup key.
 *
 * There are a handful of special cases. The operators of arity 2 are all special cases:
 *      (DO, s)
 *      (DO, STATE)
 *      (DO, DUMP)
 *      (DO, ADVANC)
 *      (c, d) ; A shortcut for (c, P, d)
 * The following operators are special cases because they are overloaded, distinguished only by the
 * arguments they are given:
 *      FR, FC, FD, DL
 *
 */
extern OperatorMap operators;
OperationData *lookup_op(const std::string_view &key, OperatorMap &map);

/**
 * @brief Test operation tokens.
 *
 * The tests are distinguished from the operators by their location within the expression: tests
 * occur immediately after a keyword (an IF expression).
 *
 */
extern OperatorMap tests;

/**
 * @brief An array holding operators that are special cases.
 */
extern const std::array<OperationData, 7> special_ops;

} // end namespace elsix
