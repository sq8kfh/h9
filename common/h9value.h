/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-12-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_H9VALUE_H
#define H9_H9VALUE_H

#include "config.h"
#include <string>


class BaseType;

class H9Value {
private:
    BaseType *value_type;
public:
    H9Value() = delete;
    H9Value(const H9Value& a) noexcept;
    H9Value(H9Value&& old) noexcept;
    ~H9Value() noexcept;
    explicit H9Value(int i) noexcept;
    explicit H9Value(const char* str) noexcept;
    explicit H9Value(const std::string& str) noexcept;

    bool is_int() const noexcept;
    bool is_str() const noexcept;

    int get_int() const noexcept;
    std::string get_str() const noexcept;
};


#endif //H9_H9VALUE_H
