/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_GENERICMSG_H_
#define _H9_GENERICMSG_H_

#include "config.h"
#include <stdexcept>
#include <string>
#include <libxml/tree.h>
#include <libxml/xmlschemastypes.h>

class GenericMsg {
public:
    enum class Type {
        GENERIC = 0,
        FRAME_RECEIVED,
        SEND_FRAME,
        SUBSCRIBE,
        ERROR,
        METHODCALL,
        METHODRESPONSE,
    };
    class InvalidMsg: public std::runtime_error {
    public:
        explicit InvalidMsg(const char* what_arg): runtime_error(what_arg) {
        };
    };
private:
    xmlDocPtr doc = nullptr;
    static xmlSchemaValidCtxtPtr valid_ctxt;
protected:
    explicit GenericMsg(GenericMsg::Type msg_type);
    xmlNodePtr get_msg_root();
public:
    explicit GenericMsg(const std::string& xml);
    GenericMsg(const GenericMsg& k);
    GenericMsg(GenericMsg&& k) noexcept;

    virtual GenericMsg::Type get_type();
    std::string serialize() const;
    bool validate_msg(std::string *error_msg = nullptr);
    virtual ~GenericMsg();
};


#endif //_H9_GENERICMSG_H_
