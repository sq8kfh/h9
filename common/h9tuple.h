/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-12-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_H9TUPLE_H
#define H9_H9TUPLE_H

#include "config.h"
#include <cstdarg>


class H9Tuple {
private:
    H9Tuple() noexcept;
public:
    static H9Tuple buildTuple(const char *format, ...);
    static H9Tuple buildTuple(const char *format, std::va_list vargs);

    H9Tuple(const H9Tuple& a) noexcept;
    H9Tuple(H9Tuple&& a) noexcept;
    ~H9Tuple();

    H9Tuple& operator=(const H9Tuple &a) noexcept;
    H9Tuple& operator=(H9Tuple&& a) noexcept;

    bool parseTuple(const char *format, ...) const;
    bool parseVa(const char *format, std::va_list vargs) const;
};


#endif //H9_H9TUPLE_H
