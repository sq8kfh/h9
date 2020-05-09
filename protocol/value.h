/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-03.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
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
    class iterator {
    private:
        xmlNode* _node;
    public:
        explicit iterator(xmlNode* node): _node(node) {}
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Value;
        using difference_type = std::ptrdiff_t;
        using pointer = Value*;
        using reference = Value&;

        bool operator!=(iterator& a) const {
            return _node != a._node;
        }

        iterator& operator++() {
            while((_node = _node->next)) {
                if (_node->type == XML_ELEMENT_NODE) {
                    break;
                }
            }
            return *this;
        }

        Value operator*() {
            return Value(_node);
        }

    };

    /*explicit*/ Value(xmlNodePtr node);
    Value& operator=(const Value& a) = delete;

    std::string get_name() const;
    void set_name(const std::string& name);

    void set_value(const char* value);
    void set_value(const std::string& value);
    template<typename value_t>
    void set_value(value_t value);

    int get_value_as_int() const;
    std::string get_value_as_str() const;

    Value add_array(const char* name);
    Value& add_value(const std::string &name, const char* value);
    Value& add_value(const std::string &name, const std::string& value);
    template<typename value_t>
    Value& add_value(const std::string &name, value_t value);

    Value operator[](const char* name);
    iterator begin();
    iterator end();
};

template<typename value_t>
void Value::set_value(value_t value) {
    set_value(std::to_string(value));
}

template<typename value_t>
Value& Value::add_value(const std::string &name, value_t value) {
    return add_value(name, std::to_string(value));
}


#endif //H9_VALUE_H
