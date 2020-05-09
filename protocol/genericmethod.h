#include <utility>

/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-30.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_GENERICMETHOD_H
#define H9_GENERICMETHOD_H

#include "concretizemsg.h"
#include "common/logger.h"
#include "value.h"


template<GenericMsg::Type msg_type, typename Derived>
class GenericMethod: public ConcretizeMsg<msg_type>  {
protected:
    GenericMethod(GenericMsg&& k): ConcretizeMsg<msg_type>(std::move(k)) {
    }
    GenericMethod(const std::string& method_name): ConcretizeMsg<msg_type>() {
        set_method_name(method_name);
    }
public:
    std::string get_method_name() {
        xmlNodePtr msg = ConcretizeMsg<msg_type>::get_msg_root();
        xmlChar *tmp;
        if ((tmp = xmlGetProp(msg, (const xmlChar *) "method")) == nullptr) {
            h9_log_err("GenericMethod: missing 'method' property");
            throw GenericMsg::InvalidMsg("missing 'method' property");
        }
        std::string ret = {reinterpret_cast<char const *>(tmp)};
        xmlFree(tmp);
        return ret;
    }

    void set_method_name(const std::string& name) {
        xmlNodePtr msg = ConcretizeMsg<msg_type>::get_msg_root();
        xmlNewProp(msg, reinterpret_cast<xmlChar const *>("method"), reinterpret_cast<xmlChar const *>(name.c_str()));
    }

    Derived& set_value(const std::string &name, const char* value) {
        xmlNodePtr msg = ConcretizeMsg<msg_type>::get_msg_root();
        xmlNodePtr val = xmlNewTextChild(msg, nullptr, reinterpret_cast<xmlChar const *>("value"), reinterpret_cast<xmlChar const *>(value));
        xmlNewProp(val, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name.c_str()));
        return dynamic_cast<Derived&>(*this);
    }

    Derived& set_value(const std::string &name, const std::string& value) {
        return set_value(name, value.c_str());
    }

    template<typename value_t>
    Derived& set_value(const std::string &name, value_t value) {
        return set_value(name, std::to_string(value));
    }

    Value set_array(const char* name) {
        xmlNodePtr msg = ConcretizeMsg<msg_type>::get_msg_root();
        xmlNodePtr array = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("array"));
        xmlNewProp(array, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name));
        xmlAddChild(msg, array);
        return Value(array);
    }

    Value operator[](const char* name) {
        xmlNodePtr msg = ConcretizeMsg<msg_type>::get_msg_root();
        for (xmlNode *node = msg->children; node; node = node->next) {
            if (node->type == XML_ELEMENT_NODE && (xmlStrcmp(node->name, reinterpret_cast<xmlChar const *>("array")) == 0 ||
                    xmlStrcmp(node->name, reinterpret_cast<xmlChar const *>("value")) == 0)) {
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
};


#endif //H9_GENERICMETHOD_H
