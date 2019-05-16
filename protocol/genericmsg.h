#ifndef _H9_GENERICMSG_H_
#define _H9_GENERICMSG_H_

#include "config.h"
#include <string>
#include <libxml/tree.h>


class GenericMsg {
public:
    enum class Type {
        FRAME_RECEIVED,
        SEND_FRAME
    };
private:
    xmlDocPtr doc = nullptr;
protected:
    explicit GenericMsg(GenericMsg::Type msg_type);
    xmlNodePtr get_msg_root();
public:
    explicit GenericMsg(const std::string& xml);
    GenericMsg(const GenericMsg& k);
    GenericMsg(GenericMsg&& k) noexcept;

    virtual GenericMsg::Type get_type();
    std::string serialize() const;
    bool validate_msg();
    virtual ~GenericMsg();
};


#endif //_H9_GENERICMSG_H_
