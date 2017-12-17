#include "h9_xmlmsg.h"
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include "h9msg.xsd.h"

#include "h9_log.h"

static int invalid_xml(xmlDocPtr doc) {
    //xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewParserCtxt("./h9msg.xsd");
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

    return res;
}

static h9msg_t *node2h9msg(xmlNode *node) {
    h9msg_t *msg = h9msg_init();

    xmlChar *tmp;
    if ((tmp = xmlGetProp(node, (const xmlChar *) "endpoint"))) {
        msg->endpoint = (char *)tmp;
    }

    if ((tmp = xmlGetProp(node, (const xmlChar *) "priority")) == NULL) {
        h9msg_free(msg);
        return NULL;
    }
    if (tmp[0] == 'H' || tmp[0] == 'h') {
        msg->priority = H9_MSG_PRIORITY_HIGH;
    } else {
        msg->priority = H9_MSG_PRIORITY_LOW;
    }
    xmlFree(tmp);

    if ((tmp = xmlGetProp(node, (const xmlChar *) "type")) == NULL) {
        h9msg_free(msg);
        return NULL;
    }
    msg->type = (uint8_t)strtol((char *)tmp, (char **)NULL, 10);
    xmlFree(tmp);

    if ((tmp = xmlGetProp(node, (const xmlChar *) "source")) == NULL) {
        h9msg_free(msg);
        return NULL;
    }
    msg->source_id = (uint16_t)strtol((char *)tmp, (char **)NULL, 10);
    xmlFree(tmp);

    if ((tmp = xmlGetProp(node, (const xmlChar *) "destination")) == NULL) {
        h9msg_free(msg);
        return NULL;
    }
    msg->destination_id = (uint16_t)strtol((char *)tmp, (char **)NULL, 10);
    xmlFree(tmp);

    if ((tmp = xmlGetProp(node, (const xmlChar *) "dlc")) == NULL) {
        h9msg_free(msg);
        return NULL;
    }
    msg->dlc = (uint8_t)strtol((char *)tmp, (char **)NULL, 10);
    xmlFree(tmp);

    if ((tmp = xmlGetProp(node, (const xmlChar *) "data"))) {
        for (int i = 0; i < strnlen((char *)tmp, 16)/2; ++i) {
            char hex_tmp[3] = {tmp[i*2], tmp[i*2+1], '\0'};
            msg->data[i] = (uint8_t)strtol(hex_tmp, (char **)NULL, 16);
        }
        xmlFree(tmp);
    }
    else if (msg->dlc != 0) {
        h9msg_free(msg);
        return NULL;
    }

    return msg;
}

int h9_xmlmsg_parse(const char *msg, size_t msg_size, int xsd_validate, void **params) {
    xmlDocPtr doc;

    doc = xmlReadMemory(msg, msg_size, "noname.xml", NULL, 0);
    if (doc == NULL) {
        h9_log_warn("Failed to parse xml msg");
        return H9_XMLMSG_UNKNOWN;
    }

    if (xsd_validate) {
        if (invalid_xml(doc)) {
            h9_log_warn("Invalid XML message");
            xmlFreeDoc(doc);
            return H9_XMLMSG_UNKNOWN;
        }
    }

    xmlNode *root_element = xmlDocGetRootElement(doc);

    int ret_msg_type = H9_XMLMSG_UNKNOWN;

    if (root_element && root_element->type == XML_ELEMENT_NODE) {
        if (strcasecmp((const char *) root_element->name, "h9methodCall") == 0) {
            ret_msg_type = H9_XMLMSG_METHODCALL;
        }
        else if (strcasecmp((const char *) root_element->name, "h9methodResponse") == 0) {
            ret_msg_type = H9_XMLMSG_H9METHODRESPONSE;
        }
        else if (strcasecmp((const char *) root_element->name, "h9sendmsg") == 0) {
            *params = node2h9msg(root_element);

            ret_msg_type = H9_XMLMSG_H9SENDMSG;
        }
        else if (strcasecmp((const char *) root_element->name, "h9msg") == 0) {
            *params = node2h9msg(root_element);

            ret_msg_type = H9_XMLMSG_H9MSG;
        }
        else {
            h9_log_warn("Invalid XML root node: %s", (const char *) root_element->name);
            xmlFreeDoc(doc);
            return H9_XMLMSG_UNKNOWN;
        }
    }
    else {
        h9_log_warn("Invalid XML root node");
        xmlFreeDoc(doc);
        return H9_XMLMSG_UNKNOWN;
    }

    xmlFreeDoc(doc);
    return ret_msg_type;
}
