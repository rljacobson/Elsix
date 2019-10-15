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

#include <iostream>
#include <string>

// #include "fmt/format.h"
#include "error.hpp"

namespace elsix{

// region: ErrorHandler Class Implementation
// region: ErrorHandler constructors

ErrorHandler::ErrorHandler():
    errors_(ErrorVector()), err_stream_(&std::cerr), err_log_stream_(nullptr){
}

ErrorHandler::ErrorHandler(std::ostream &err, std::ostream &log):
    errors_(std::vector< Error >()), err_stream_(&err), err_log_stream_(&log){
}

ErrorHandler::ErrorHandler(std::ostream &err):
    errors_(std::vector< Error >()), err_stream_(&err), err_log_stream_(nullptr){
}
// endregion: ErrorHandler constructors

void ErrorHandler::emitError(const std::string &&message, Span span){
    errors_.emplace_back(std::forward<const std::string>(message), span);
    outputError(errors_.back());
}

void ErrorHandler::emitFatalError(const std::string &&message, Span span){
    emitError(std::forward<const std::string>(message), span);
    throw(FatalException(errors_.back()));
}

const ErrorVector &ErrorHandler::getErrors() const noexcept{
    return errors_;
}

void ErrorHandler::outputError(const Error &error) const noexcept{
    if(err_log_stream_ != nullptr){
        *err_log_stream_ << error.message << std::endl;
    }
    if(err_stream_ != nullptr){
        *err_stream_ << error.message << std::endl;
    }
}

void ErrorHandler::setLogStream(std::ostream &log) noexcept{
    err_log_stream_ = &log;
}

const std::ostream &ErrorHandler::getLogStream() const noexcept{
    return *err_log_stream_;
}
// endregion: ErrorHandler Implementation

// region: Error Class Implementation

Error::Error(const std::string &&msg, Span spn):
    span(spn), message(std::forward<const std::string>(msg)){
}

// endregion: Error Class Implementation

// region: FatalException Class Implementation

const char *FatalException::what() const noexcept {
    return error.message.c_str();
}

FatalException::FatalException(const Error &e) :
    error(e){
}
// endregion: FatalException Class Implementation

}
