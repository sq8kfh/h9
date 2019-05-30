#include <utility>

/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-30.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_GENERICMETHOD_H
#define H9_GENERICMETHOD_H

#include "concretizemsg.h"


template<GenericMsg::Type msg_type, const char* arg_node_name, typename Derived>
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
        if ((tmp = xmlGetProp(msg, (const xmlChar *) "name")) == nullptr) {
            h9_log_err("GenericMethod: missing 'name' property");
            throw GenericMsg::InvalidMsg("missing 'name' property");
        }
        std::string ret = {reinterpret_cast<char const *>(tmp)};
        xmlFree(tmp);
        return ret;
    }

    void set_method_name(const std::string& name) {
        xmlNodePtr msg = ConcretizeMsg<msg_type>::get_msg_root();
        xmlNewProp(msg, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(name.c_str()));
    }

    int parse_arg(const char *format, ...) {
        xmlNodePtr msg = ConcretizeMsg<msg_type>::get_msg_root();
        va_list vargs;
        va_start(vargs, format);

        const char *p = format;
        for (xmlNode *node = msg->children; node; node = node->next) {
            if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name,reinterpret_cast<xmlChar const *>(arg_node_name)) == 0) {
                xmlNodePtr text_node = node->children;
                if(text_node->type == XML_TEXT_NODE) {
                    xmlChar* tmp = xmlNodeGetContent(text_node);

                    switch (*p) {
                        case 'S': {
                            std::string* s = va_arg(vargs, std::string*);
                            *s = std::string(reinterpret_cast<char*>(tmp));
                            break;
                        }
                        case 'i': {
                            int *i = va_arg(vargs, int*);
                            *i = std::stoi(reinterpret_cast<const char*>(tmp), nullptr, 10);
                            break;
                        }
                        case 'u': {
                            unsigned int *i = va_arg(vargs, unsigned int*);
                            *i = std::stoul(reinterpret_cast<const char*>(tmp), nullptr, 10);
                            break;
                        }
                        default:
                            throw std::invalid_argument("GenericMethod::parse_arg: unknown argument: " + std::string(1, *p));
                    }

                    xmlFree(tmp);
                }
                ++p;
            }
        }

        va_end(vargs);
        return 0;
    }

    Derived& build_arg(const char *format, ...) {
        xmlNodePtr msg = ConcretizeMsg<msg_type>::get_msg_root();
        va_list vargs;
        va_start(vargs, format);

        for (const char *p = format; *p != '\0'; ++p) {
            std::string val;
            switch (*p) {
                case 's': {
                    const char *s = va_arg(vargs, const char*);
                    val = s;
                    break;
                }
                case 'i': {
                    int i = va_arg(vargs, int);
                    val = std::to_string(i);
                    break;
                }
                case 'u': {
                    unsigned int u = va_arg(vargs, unsigned int);
                    val = std::to_string(u);
                    break;
                }
                default:
                    throw std::invalid_argument("GenericMethod::build_arg: unknown argument: " + std::string(1, *p));
            }
            xmlNewTextChild(msg, nullptr, reinterpret_cast<xmlChar const *>(arg_node_name), reinterpret_cast<xmlChar const *>(val.c_str()));
        }

        va_end(vargs);

        return dynamic_cast<Derived&>(*this);
    }
};


#endif //H9_GENERICMETHOD_H
