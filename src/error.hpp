#include <utility>

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

#include <iosfwd>
#include <vector>
#include <string>

#include "location.hpp"

// ToDo: Is this the right place for this define?
#define UNREACHABLE     __builtin_unreachable();\
                        assert(false);

namespace elsix{

enum class ErrorType{
    TYPE_MISMATCH,
    UNEXPECTED_TOKEN
};

struct Error{
    Error(const std::string &&msg, Span spn);
    const Span span;
    const std::string &message;
};

using ErrorVector = std::vector< Error >;

class ErrorHandler{
public:
    ErrorHandler();
    explicit ErrorHandler(std::ostream &err);
    explicit ErrorHandler(std::ostream &err, std::ostream &log);

    void emitError(const std::string &&message, Span span);
    void emitFatalError(const std::string &&message, Span span);

    [[nodiscard]] const ErrorVector &getErrors() const noexcept;
    void setLogStream(std::ostream &log) noexcept;
    [[nodiscard]] const std::ostream &getLogStream() const noexcept;

private:
    ErrorVector errors_;
    std::ostream *err_stream_;
    std::ostream *err_log_stream_;

    void outputError(const Error &error) const noexcept;
};

class FatalException: std::exception{
public:
    explicit FatalException(const Error &e);
    const Error &error;
    [[nodiscard]] const char *what() const noexcept override;
};

}
