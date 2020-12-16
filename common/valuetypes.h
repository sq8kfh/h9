/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-12-10.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_VALUETYPES_H
#define H9_VALUETYPES_H

#include "config.h"
#include <string>


class BaseType {
public:
    virtual ~BaseType() = default;
    virtual BaseType* clone() const = 0;
};


template <typename T>
class ScalarType: public BaseType {
    T value;
public:
    explicit ScalarType(const T& v);
    BaseType* clone() const override;
    T get_value() const {
        return value;
    }
};


using StrScalar = ScalarType<std::string>;
using IntScalar = ScalarType<int>;


template<typename T>
ScalarType<T>::ScalarType(const T& v): value(v) {
}

template<typename T>
BaseType *ScalarType<T>::clone() const {
    return new ScalarType<T>(value);
}


#endif //H9_VALUETYPES_H
