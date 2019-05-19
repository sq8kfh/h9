/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-18.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_FRAMEMSG_H_
#define _H9_FRAMEMSG_H_

#include <cstdio>
#include <string>
#include "common/logger.h"
#include "concretizemsg.h"
#include "bus/h9frame.h"


template<GenericMsg::Type msg_type>
class FrameMsg: public ConcretizeMsg<msg_type> {
protected:
    FrameMsg(GenericMsg&& k): ConcretizeMsg<msg_type>(std::move(k)) {
    }

    FrameMsg(const H9frame& frame): ConcretizeMsg<msg_type>() {
        xmlNodePtr root = ConcretizeMsg<msg_type>::get_msg_root();

        if (frame.priority == H9frame::Priority::HIGH) {
            xmlNewProp(root, reinterpret_cast<xmlChar const *>("priority"), reinterpret_cast<xmlChar const *>("H"));
        }
        else {
            xmlNewProp(root, reinterpret_cast<xmlChar const *>("priority"), reinterpret_cast<xmlChar const *>("L"));
        }
        constexpr int str_sizie = 24;
        char str[str_sizie];
        std::snprintf(str, str_sizie, "%u", H9frame::to_underlying<H9frame::Type, unsigned int>(frame.type));
        xmlNewProp(root, reinterpret_cast<xmlChar const *>("type"), reinterpret_cast<xmlChar const *>(str));
        std::snprintf(str, str_sizie, "%u", (unsigned int)frame.seqnum);
        xmlNewProp(root, reinterpret_cast<xmlChar const *>("seqnum"), reinterpret_cast<xmlChar const *>(str));
        std::snprintf(str, str_sizie, "%u", (unsigned int)frame.source_id);
        xmlNewProp(root, reinterpret_cast<xmlChar const *>("source"), reinterpret_cast<xmlChar const *>(str));
        std::snprintf(str, str_sizie, "%u", (unsigned int)frame.destination_id);
        xmlNewProp(root, reinterpret_cast<xmlChar const *>("destination"), reinterpret_cast<xmlChar const *>(str));
        std::snprintf(str, str_sizie, "%u", (unsigned int)frame.dlc);
        xmlNewProp(root, reinterpret_cast<xmlChar const *>("dlc"), reinterpret_cast<xmlChar const *>(str));

        for (int i = 0; i < frame.dlc; i++) {
            std::snprintf(&str[i*2], str_sizie, "%02hhX", frame.data[i]);
        }
        if (frame.dlc) {
            xmlNewProp(root, reinterpret_cast<xmlChar const *>("data"), reinterpret_cast<xmlChar const *>(str));
        }
    }
public:
    H9frame get_frame() {
        xmlNodePtr root = ConcretizeMsg<msg_type>::get_msg_root();

        xmlChar *tmp;
        H9frame frame;

        if ((tmp = xmlGetProp(root, (const xmlChar *) "priority")) == nullptr) {
            h9_log_err("SendFrameMsg: missing 'priority' property");
        }
        if (tmp[0] == 'H' || tmp[0] == 'h') {
            frame.priority = H9frame::Priority::HIGH;
        } else {
            frame.priority = H9frame::Priority::LOW;
        }
        xmlFree(tmp);

        if ((tmp = xmlGetProp(root, (const xmlChar *) "type")) == nullptr) {
            h9_log_err("SendFrameMsg: missing 'type' property");
        }
        frame.set_type_from_underlying(strtol((char *)tmp, (char **)nullptr, 10));
        //frame.type = static_cast<H9frame::Type >(strtol((char *)tmp, (char **)nullptr, 10));
        xmlFree(tmp);

        if ((tmp = xmlGetProp(root, (const xmlChar *) "seqnum")) == nullptr) {
            h9_log_err("SendFrameMsg: missing 'seqnum' property");
        }
        frame.seqnum = (uint8_t)strtol((char *)tmp, (char **)nullptr, 10);
        xmlFree(tmp);

        if ((tmp = xmlGetProp(root, (const xmlChar *) "source")) == nullptr) {
            h9_log_err("SendFrameMsg: missing 'source' property");
        }
        frame.source_id = (uint16_t)strtol((char *)tmp, (char **)nullptr, 10);
        xmlFree(tmp);

        if ((tmp = xmlGetProp(root, (const xmlChar *) "destination")) == nullptr) {
            h9_log_err("SendFrameMsg: missing 'destination' property");
        }
        frame.destination_id = (uint16_t)strtol((char *)tmp, (char **)nullptr, 10);
        xmlFree(tmp);

        if ((tmp = xmlGetProp(root, (const xmlChar *) "dlc")) == nullptr) {
            h9_log_err("SendFrameMsg: missing 'dlc' property");
        }
        frame.dlc = (uint8_t)strtol((char *)tmp, (char **)nullptr, 10);
        xmlFree(tmp);

        if ((tmp = xmlGetProp(root, (const xmlChar *) "data"))) {
            for (int i = 0; i < strnlen((char *)tmp, 16)/2; ++i) {
                xmlChar hex_tmp[3] = {tmp[i*2], tmp[i*2+1], '\0'};
                frame.data[i] = (uint8_t)strtol((char *)hex_tmp, (char **)nullptr, 16);
            }
            xmlFree(tmp);
        }
        else if (frame.dlc != 0) {
            h9_log_err("SendFrameMsg: 'dlc' and 'data' mismatch");
        }

        return frame;
    }
};


#endif //_H9_FRAMEMSG_H_
