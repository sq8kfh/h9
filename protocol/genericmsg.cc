/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "genericmsg.h"

#include <cassert>
#include <algorithm>

#include <libxml/parser.h>
#include <libxml/xmlerror.h>
#include "common/logger.h"
#include "h9msg.xsd.h"


xmlSchemaValidCtxtPtr GenericMsg::valid_ctxt = nullptr;

GenericMsg::GenericMsg(GenericMsg::Type msg_type) {
    doc = xmlNewDoc(reinterpret_cast<xmlChar const *>("1.0"));
    xmlNodePtr root = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("h9"));
    xmlNewProp(root, reinterpret_cast<xmlChar const *>("version"), reinterpret_cast<xmlChar const *>("0.0"));
    xmlNodePtr node = nullptr;
    switch (msg_type) {
        case Type::GENERIC:
            throw std::invalid_argument("Trying create a GENERIC message");
            break;
        case Type::FRAME_RECEIVED:
            node = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("frame_received"));
            break;
        case Type::SEND_FRAME:
            node = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("send_frame"));
            break;
        case Type::SUBSCRIBE:
            node = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("subscribe"));
            break;
        case Type::ERROR:
            node = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("error"));
            break;
        case Type::METHODCALL:
            node = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("methodcall"));
            break;
        case Type::METHODRESPONSE:
            node = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("methodresponse"));
            break;
    }
    xmlAddChild(root, node);
    xmlDocSetRootElement(doc, root);
}

xmlNodePtr GenericMsg::get_msg_root() {
    for (xmlNode *cur_node = xmlDocGetRootElement(doc)->children; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            return cur_node;
        }
    }
    return nullptr;
}

GenericMsg::GenericMsg(const GenericMsg& k) {
#ifdef H9_DEBUG
    h9_log_warn("GenericMsg: exec copy constructor");
#else
    h9_log_debug("GenericMsg: exec copy constructor");
#endif
    if (k.doc) {
        doc = xmlCopyDoc(k.doc, 1);
    }
}

GenericMsg::GenericMsg(GenericMsg&& k) noexcept {
    //h9_log_debug("GenericMsg: exec move constructor");
    doc = k.doc;
    k.doc = nullptr;
}

GenericMsg::GenericMsg(const std::string& xml) {
    doc = xmlReadMemory(xml.c_str(), xml.length(), "noname.xml", nullptr, 0);
    if (doc == nullptr) {
        fprintf(stderr, "Failed to parse document\n");
        return;
    }
}

GenericMsg::Type GenericMsg::get_type() {
    xmlNodePtr node = get_msg_root();
    assert(node);

    if (node) {
        if (xmlStrcmp(node->name,reinterpret_cast<xmlChar const *>("frame_received")) == 0)
            return Type::FRAME_RECEIVED;
        else if (xmlStrcmp(node->name,reinterpret_cast<xmlChar const *>("send_frame")) == 0)
            return Type::SEND_FRAME;
        else if (xmlStrcmp(node->name,reinterpret_cast<xmlChar const *>("subscribe")) == 0)
            return Type::SUBSCRIBE;
        else if (xmlStrcmp(node->name,reinterpret_cast<xmlChar const *>("error")) == 0)
            return Type::ERROR;
        else if (xmlStrcmp(node->name,reinterpret_cast<xmlChar const *>("methodcall")) == 0)
            return Type::METHODCALL;
        else if (xmlStrcmp(node->name,reinterpret_cast<xmlChar const *>("methodresponse")) == 0)
            return Type::METHODRESPONSE;
    }
    return Type::GENERIC;
}

std::string GenericMsg::serialize() const {
    xmlChar *xmlbuff = nullptr;
    int buffersize = 0;
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);

    std::string ret = std::string(reinterpret_cast<const char *>(xmlbuff));
    xmlFree(xmlbuff);

    /*
    xmlBufferPtr buffer = xmlBufferCreate();

    //xml_length = xmlNodeDump(buffer, NULL, node, 0, 1);
    int res = xmlNodeDump(buffer, doc, xmlmsg->node, 0, 1);
    char *ret = NULL;

    if (res > 0) {
        ret = strdup((char*) buffer->content);
        *xml_length = (size_t)res;
        //h9_log_debug("xmlmsg build(%d): %s", *xml_length, ret);
    }

    xmlUnlinkNode(xmlmsg->node);

    xmlBufferFree(buffer);
    xmlFreeDoc(doc);*/

    return std::move(ret);
}

void xml_error_func(void * ctx, const char* msg, ...) {
    /* /dev/null :P */
}

bool GenericMsg::validate_msg(std::string *error_msg) {
    if (valid_ctxt == nullptr) {
        //xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewParserCtxt("./h9msg.xsd");
        xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewMemParserCtxt((char*)h9msg_xsd, h9msg_xsd_len);
        if (parser_ctxt == nullptr) {
            h9_log_crit("unable to create a parser context for the schema");
            exit(EXIT_FAILURE);
        }
        xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
        if (schema == nullptr) {
            xmlSchemaFreeParserCtxt(parser_ctxt);
            h9_log_crit("the schema itself is not valid");
            exit(EXIT_FAILURE);
        }
        valid_ctxt = xmlSchemaNewValidCtxt(schema);
        if (valid_ctxt == nullptr) {
            xmlSchemaFree(schema);
            xmlSchemaFreeParserCtxt(parser_ctxt);
            h9_log_crit("unable to create a validation context for the schema");
            exit(EXIT_FAILURE);
        }
        //xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
    }

    xmlGenericErrorFunc tmp = xml_error_func;
    initGenericErrorDefaultFunc(&tmp);

    //TODO: verify a memory state
    int res = xmlSchemaValidateDoc(valid_ctxt, doc);

    initGenericErrorDefaultFunc(nullptr);

    if (error_msg) {
        if (res) {
            xmlErrorPtr err_object = xmlGetLastError();
            *error_msg = err_object->message;
            error_msg->erase(std::find_if(error_msg->rbegin(), error_msg->rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), error_msg->end());
        }
        else {
            error_msg->clear();
        }
    }
    //xmlSchemaFreeValidCtxt(valid_ctxt);
    //xmlSchemaFree(schema);
    //xmlSchemaFreeParserCtxt(parser_ctxt);
    //xmlCleanupParser();

    return res;
}

GenericMsg::~GenericMsg() {
    if (doc) {
        xmlFreeDoc(doc);
    }
}
