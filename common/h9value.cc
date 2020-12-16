/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-12-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "h9value.h"
#include "valuetypes.h"


H9Value::H9Value(const H9Value &a) noexcept {
    value_type = a.value_type->clone();
}

H9Value::H9Value(H9Value &&old) noexcept {
    value_type = old.value_type;
    old.value_type = nullptr;
}

H9Value::~H9Value() noexcept {
    delete value_type;
}

H9Value::H9Value(int i) noexcept {
    value_type = new IntScalar(i);
}

H9Value::H9Value(const char *str) noexcept {
    value_type = new StrScalar(str);
}

H9Value::H9Value(const std::string &str) noexcept {
    value_type = new StrScalar(str);
}

bool H9Value::is_int() const noexcept {
    return dynamic_cast<IntScalar*>(value_type) != nullptr;
}

bool H9Value::is_str() const noexcept {
    return dynamic_cast<StrScalar*>(value_type) != nullptr;
}

int H9Value::get_int() const noexcept {
    return dynamic_cast<IntScalar*>(value_type)->get_value();
}

std::string H9Value::get_str() const noexcept {
    return dynamic_cast<StrScalar*>(value_type)->get_value();
}
