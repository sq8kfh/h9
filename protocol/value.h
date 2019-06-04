/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-03.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_VALUE_H
#define H9_VALUE_H

#include "config.h"

#include <string>
#include <libxml/tree.h>
#include <libxml/xmlschemastypes.h>

class Value {
private:
    xmlNode* const _node;
public:
    Value(xmlNodePtr node);
    int as_int();
    std::string as_string();
    Value operator[](const char* name);
    Value& set_value(const std::string &name, const char* value);
    Value& set_value(const std::string &name, const std::string& value);

    template<typename value_t>
    Value& set_value(const std::string &name, value_t value) {
        return set_value(name, std::to_string(value));
    }
};


#endif //H9_VALUE_H
