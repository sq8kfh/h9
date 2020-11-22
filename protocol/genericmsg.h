/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
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
        FRAME,
        SEND_FRAME,
        SUBSCRIBE,
        ERROR,
        CALL,
        RESPONSE,
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
    GenericMsg() noexcept;
    explicit GenericMsg(const std::string& xml);
    GenericMsg(const GenericMsg& k);
    GenericMsg(GenericMsg&& k) noexcept;
    GenericMsg& operator=(const GenericMsg& k) noexcept;
    GenericMsg& operator=(GenericMsg&& k) noexcept;

    virtual GenericMsg::Type get_type();
    std::uint64_t get_id(void) const;
    void set_id(std::uint64_t id);
    std::uint64_t get_request_id(void) const;
    void set_request_id(std::uint64_t id);
    std::string serialize() const;
    bool validate_msg(std::string *error_msg = nullptr);
    virtual ~GenericMsg();

    void* id() const {
        return doc;
    };
};


#endif //_H9_GENERICMSG_H_
