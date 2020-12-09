/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-12-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "h9tuple.h"


H9Tuple H9Tuple::buildTuple(const char *format, ...) {
    va_list vargs;
    va_start(vargs, format);
    auto ret = buildTuple(format, vargs);
    va_end(vargs);
    return ret;
}

H9Tuple H9Tuple::buildTuple(const char *format, std::va_list vargs) {

}

H9Tuple::H9Tuple() noexcept {

}

H9Tuple::H9Tuple(const H9Tuple &a) noexcept {

}

H9Tuple::H9Tuple(H9Tuple &&a) noexcept {

}

H9Tuple::~H9Tuple() {

}

H9Tuple &H9Tuple::operator=(const H9Tuple &a) noexcept {
    return *this;
}

H9Tuple &H9Tuple::operator=(H9Tuple &&a) noexcept {
    return *this;
}

bool H9Tuple::parseTuple(const char *format, ...) const {
    va_list vargs;
    va_start(vargs, format);
    auto ret = parseVa(format, vargs);
    va_end(vargs);
    return ret;
}

bool H9Tuple::parseVa(const char *format, std::va_list vargs) const {
    return false;
}
