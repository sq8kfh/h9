#include "genericmsg.h"

#include <libxml/parser.h>
#include "common/logger.h"

GenericMsg::GenericMsg(GenericMsg::Type msg_type) {
    doc = xmlNewDoc(reinterpret_cast<xmlChar const *>("1.0"));
    xmlNodePtr node = nullptr;
    switch (msg_type) {
        case Type::FRAME_RECEIVED:
            node = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("h9msg"));
            break;
        case Type::SEND_FRAME:
            node = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("h9msg"));
            break;
    }
    xmlDocSetRootElement(doc, node);
}

xmlNodePtr GenericMsg::get_msg_root() {
    return xmlDocGetRootElement(doc);
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
    h9_log_debug("GenericMsg: exec move constructor");
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
    return Type::SEND_FRAME;
}

bool GenericMsg::validate_msg() {
    /*//xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewParserCtxt("./h9msg.xsd");
    xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewMemParserCtxt((char*)h9msg_xsd, h9msg_xsd_len);
    if (parser_ctxt == NULL) {
        h9_log_crit("unable to create a parser context for the schema");
        exit(EXIT_FAILURE);
    }
    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL) {
        xmlSchemaFreeParserCtxt(parser_ctxt);
        h9_log_crit("the schema itself is not valid");
        exit(EXIT_FAILURE);
    }
    xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL) {
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_ctxt);
        h9_log_crit("unable to create a validation context for the schema");
        exit(EXIT_FAILURE);
    }

    int res = xmlSchemaValidateDoc(valid_ctxt, doc);

    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlCleanupParser();

    return res;*/
    return true;
}

GenericMsg::~GenericMsg() {
    if (doc) {
        xmlFreeDoc(doc);
    }
}
