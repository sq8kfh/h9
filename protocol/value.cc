/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-03.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "value.h"
#include "genericmethod.h"

Value::Value(xmlNodePtr node): _node(node) {
}

std::string Value::get_name() const {
    xmlChar *tmp;
    if ((tmp = xmlGetProp(_node, reinterpret_cast<xmlChar const *>("name"))) == nullptr) {
        return "";
    }
    std::string ret = {reinterpret_cast<char const *>(tmp)};
    xmlFree(tmp);
    return ret;
}

//void Value::set_name(const std::string& name) {
//    xmlSetProp(_node, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name.c_str()));
//}
//
//void Value::set_value(const char* value) {
//    xmlNodeSetContent(_node, reinterpret_cast<xmlChar const *>(value));
//}
//
//void Value::set_value(const std::string& value) {
//    set_value(value.c_str());
//}

int Value::get_value_as_int() const {
    if (_node->children && _node->children->type == XML_TEXT_NODE) {
        xmlChar* tmp = xmlNodeGetContent(_node->children);
        int ret = std::stoi(reinterpret_cast<const char*>(tmp), nullptr, 10);
        xmlFree(tmp);
        return ret;
    }
    throw std::invalid_argument("_node->children-is not a XML_TEXT_NODE");
}

std::string Value::get_value_as_str() const {
    if (_node->children && _node->children->type == XML_TEXT_NODE) {
        xmlChar* tmp = xmlNodeGetContent(_node->children);
        std::string ret = {reinterpret_cast<const char*>(tmp)};
        xmlFree(tmp);
        return std::move(ret);
    }
    throw std::invalid_argument("_node->children-is not a XML_TEXT_NODE");
}

//Value Value::add_array(const char* name) {
//    xmlNodePtr array = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("array"));
//    xmlNewProp(array, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name));
//    xmlAddChild(_node, array);
//    return Value(array);
//}
//
//Value& Value::add_value(const std::string &name, const char* value) {
//    xmlNodePtr val = xmlNewTextChild(_node, nullptr, reinterpret_cast<xmlChar const *>("value"), reinterpret_cast<xmlChar const *>(value));
//    xmlNewProp(val, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name.c_str()));
//    return *this;
//}
//
//Value& Value::add_value(const std::string &name, const std::string& value) {
//    return add_value(name, value.c_str());
//}

Value Value::operator[](const char* name) {
    for (xmlNode *node = _node->children; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE /*&& xmlStrcmp(node->name, reinterpret_cast<xmlChar const *>("value")) == 0*/) {
            xmlChar *tmp = xmlGetProp(node, (const xmlChar *) "name");
            if (tmp) {
                if (xmlStrcmp(tmp, reinterpret_cast<xmlChar const *>(name)) == 0) {
                    Value res = {node};
                    xmlFree(tmp);
                    return res;
                }
                xmlFree(tmp);
            }
        }
    }
    throw std::out_of_range(name);
}

Value::iterator Value::begin() {
    if (_node->children) {
        for (xmlNode *i = _node->children; i; i = i->next) {
            if (i->type == XML_ELEMENT_NODE) {
                return iterator(i);
            }
        }
    }
    return iterator(nullptr);
}

Value::iterator Value::end() {
    return iterator(nullptr);
}

ArrayValue::ArrayValue(xmlNodePtr node): Value(node) {
}

ArrayValue ArrayValue::add_array() {
    xmlNodePtr array = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("array"));
    xmlAddChild(_node, array);
    return ArrayValue(array);
}

DictValue ArrayValue::add_dict() {
    xmlNodePtr array = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("dict"));
    xmlAddChild(_node, array);
    return DictValue(array);
}

Value& ArrayValue::add_value(const char* value) {
    xmlNewTextChild(_node, nullptr, reinterpret_cast<xmlChar const *>("value"), reinterpret_cast<xmlChar const *>(value));
    return *this;
}

Value& ArrayValue::add_value(const std::string& value) {
    return add_value(value.c_str());
}

DictValue::DictValue(xmlNodePtr node): Value(node) {
}

ArrayValue DictValue::add_array(const char* name) {
    xmlNodePtr array = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("array"));
    xmlNewProp(array, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name));
    xmlAddChild(_node, array);
    return ArrayValue(array);
}

DictValue DictValue::add_dict(const char* name) {
    xmlNodePtr array = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("dict"));
    xmlNewProp(array, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name));
    xmlAddChild(_node, array);
    return DictValue(array);
}

Value& DictValue::add_value(const std::string &name, const char* value) {
    xmlNodePtr val = xmlNewTextChild(_node, nullptr, reinterpret_cast<xmlChar const *>("value"), reinterpret_cast<xmlChar const *>(value));
    xmlNewProp(val, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name.c_str()));
    return *this;
}

Value& DictValue::add_value(const std::string &name, const std::string& value) {
    return add_value(name, value.c_str());
}
