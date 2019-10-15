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


/**
 * @brief Header file containing database of operator information.
 *
 * This header file contains entries of the form `OPERATOR(x, y, z, w)`.
 * To use the header, define an `OPERATOR` macro taking that form,
 * include the header to "write" the operator code, and then undefine
 * the `OPERATOR` macro. (Note that we exclude include guards so we
 * can use the header more than once.
 *
 * The macros are:
 *      OPERATOR
 *      SHIFT_OPERATOR
 *
 * See below for details. Operators NOT covered by this file:
 *
 * Setup Storage
 * (s1, SS, d, s2) ; (first_word, SS, size, last_word
 *
 * Define Field
 * (cd1, Df, cd2, cd2) ; (word, Df, first_bit, last_bit)
 *
 * Get Block
 * (a, GT, cd)
 * (a, GT, cd, a2)
 *
 * Free Block
 * (a, FR, 0) ; But included as (dest, FR, source)
 *
 * (source, dest) ; Short or (source, P, dest), which is included.
 *
 * Input
 * (a, IN, cd)
 *
 * Print
 * (cd, PR, co)
 * (cd, PRH, h)
 *
 * Print List
 * (c, PL, f)
 * (c, PL, f, cd)
 *
 * Punch
 * (cd, PU, co)
 * (cd, PUH, h)
 *
 * Microfilm
 * (cdxmin, XR, cdxmax)
 * (cdymin, YR, cdymax)
 * (cdx0, TVH, cdy0, co, cd)
 * (cdx0, TVHH, cdy0, h, cd)
 * (DO, ADVANC)
 *
 * PUSHDOWN AND POP-UP OPERATIONS
 *
 * Save and
 * Restore Field
 * Contents
 * (S, FC, c)
 * (R, FC, c)
 *
 *
 * Do Subroutine
 * (DO, s)
 * (s2, DO, s)
 * (DO, STATE)
 * (DO, DUMP)
 * P ; Included as a LOC_OPERATOR, but not as a TEST_OPERATOR.
 *
 */


/**
 * Boolean comparison operators.
 *
 * OPERATOR/TEST_OPERATOR:
 *      For an operator of the form
 *
 *          (dest, op, source)
 *
 *      dest is a location (bug or remote field), op is the operation
 *      code, and source is _either_ a destination field _or_ a
 *      literal. The macro syntax is
 *
 *          OPERATOR(op, default_type, explicit_type_1, explicit_type_2)
 *
 *      where either a source location (bug, or remote field) or a
 *      literal of type `default_type` is assumed if no type specifier
 *      is given, and `explicit_type_[1|2]` are the other possible type
 *      specifiers.
 *
 */


/**
 * @brief Test operators ("Tests"): Equals, not equal, greater, less, 
 *        "one-bits of", "one-bits of"
 *
 * Note that op codes are repeated between "tests" and "operators." Their location
 * within an IF or THEN statement determines the meaning.
 * 
 */
TEST_OPERATOR(E, D, O, H)
TEST_OPERATOR(N, D, O, H)
TEST_OPERATOR(G, D, O, H)
TEST_OPERATOR(L, D, O, H)
TEST_OPERATOR(O, O, D, H)
TEST_OPERATOR(Z, O, D, H)

/**
 * @brief Arithmetic operators: Equal (Assignment), add, subtract, multiply, divide
 */
OPERATOR(E, D, O, H)
OPERATOR(A, D, O, H)
OPERATOR(S, D, O, H)
OPERATOR(M, D, O, H)
OPERATOR(V, D, O, H)

/**
 * @brief Logical operators: or, and, xor, complement
 */
OPERATOR(O, O, D, H)
OPERATOR(N, O, D, H)
OPERATOR(X, O, D, H)
OPERATOR(C, O, D, H)

/**
 * @brief Left and right shift, but these are ternary.
 *
 * SHIFT_OPERATOR:
 *      For the shift operators of the form
 *
 *          (dest, op, source)
 *      or
 *          (dest, op, source, amount)
 *
 *      dest is a location (bug or remote field), op is the operation
 *      code, and source is _either_ a destination field _or_ a decimal
 *      literal. The macro syntax is
 *
 *          SHIFT_OPERATOR(op)
 *
 *      Note that for the shift operators, the source is either a
 *      location or an octal literal, and the other types (which must
 *      be specified) are decimal and Hollerith.
 */
SHIFT_OPERATOR(L)
SHIFT_OPERATOR(R)

/**
 * @brief Operators that take only locations on either side.
 *
 * LOC_OPERATOR:
 *      Many operators have the simple form
 *
 *          (dest, op, source)
 *
 *      where dest and source are locations (bugs or remote fields).
 *      The macro syntax is
 *
 *          LOC_OPERATOR(op)
 *
 */
LOC_OPERATOR(FR)
LOC_OPERATOR(DP)
LOC_OPERATOR(IC)
LOC_OPERATOR(P)
LOC_OPERATOR(LO)
LOC_OPERATOR(LZ)
LOC_OPERATOR(RO)
LOC_OPERATOR(RZ)
LOC_OPERATOR(OS)
LOC_OPERATOR(ZS)
LOC_OPERATOR(BZ)
LOC_OPERATOR(ZB)
LOC_OPERATOR(BD)
LOC_OPERATOR(BO)
LOC_OPERATOR(DB)
LOC_OPERATOR(OB)

#undef TEST_OPERATOR
#undef OPERATOR
#undef SHIFT_OPERATOR
#undef LOC_OPERATOR
