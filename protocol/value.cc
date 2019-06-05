/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-03.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */
#include "value.h"
#include "genericmethod.h"

Value::Value(xmlNodePtr node): _node(node) {

}

int Value::as_int() {
    if (_node->children && _node->children->type == XML_TEXT_NODE) {
        xmlChar* tmp = xmlNodeGetContent(_node->children);
        int ret = std::stoi(reinterpret_cast<const char*>(tmp), nullptr, 10);
        xmlFree(tmp);
        return ret;
    }
    throw std::invalid_argument("_node->children-is not a XML_TEXT_NODE");
}

std::string Value::as_string() {
    if (_node->children && _node->children->type == XML_TEXT_NODE) {
        xmlChar* tmp = xmlNodeGetContent(_node->children);
        std::string ret = {reinterpret_cast<const char*>(tmp)};
        xmlFree(tmp);
        return ret;
    }
    throw std::invalid_argument("_node->children-is not a XML_TEXT_NODE");
}

Value Value::operator[](const char* name) {
    for (xmlNode *node = _node->children; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, reinterpret_cast<xmlChar const *>("value")) == 0) {
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

Value& Value::set_value(const std::string &name, const char* value) {
    xmlNodePtr val = xmlNewTextChild(_node, nullptr, reinterpret_cast<xmlChar const *>("value"), reinterpret_cast<xmlChar const *>(value));
    xmlNewProp(val, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name.c_str()));
    return *this;
}

Value& Value::set_value(const std::string &name, const std::string& value) {
    return set_value(name, value.c_str());
}

Value Value::set_array(const char* name) {
    xmlNodePtr array = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("array"));
    xmlNewProp(array, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name));
    xmlAddChild(_node, array);
    return Value(array);
}
