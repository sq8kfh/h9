#include "h9_xmlmsg.h"
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include "h9msg.xsd.h"

#include "h9_log.h"

static int invalid_xml(xmlDocPtr doc);
static h9msg_t *node2h9msg(xmlNode *node);
static xmlNode *h9msg2node(h9msg_t *msg, const char *node_name);
static char *build_h9doc(size_t *xml_length, xmlNode *node, int xsd_validate);

int h9_xmlmsg_parse(const char *msg, size_t msg_size, void **params, int xsd_validate) {
    xmlDocPtr doc;

    *params = NULL;

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
            ret_msg_type = H9_XMLMSG_METHODRESPONSE;
        }
        else if (strcasecmp((const char *) root_element->name, "h9sendmsg") == 0) {
            *params = node2h9msg(root_element);

            ret_msg_type = H9_XMLMSG_SENDMSG;
        }
        else if (strcasecmp((const char *) root_element->name, "h9msg") == 0) {
            *params = node2h9msg(root_element);

            ret_msg_type = H9_XMLMSG_MSG;
        }
        else if (strcasecmp((const char *) root_element->name, "h9subscribe") == 0){
            ret_msg_type = H9_XMLMSG_SUBSCRIBE;
        }
        else if (strcasecmp((const char *) root_element->name, "h9unsubscribe") == 0){
            ret_msg_type = H9_XMLMSG_UNSUBSCRIBE;
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

void h9_xmlmsg_free_parse_data(int parse_result, void *parse_data) {
    if (parse_data != NULL) {
        switch (parse_result) {
            case H9_XMLMSG_SENDMSG:
            case H9_XMLMSG_MSG:
                h9msg_free(parse_data);
                break;
        }
    }
}

char *h9_xmlmsg_build_h9methodCall(size_t *xml_length, int xsd_validate) {
    return NULL;
}

char *h9_xmlmsg_build_h9methodResponse(size_t *xml_length, int xsd_validate) {
    return NULL;
}

char *h9_xmlmsg_build_h9subscribe(size_t *xml_length, char *event, int xsd_validate) {
    xmlNode *node = xmlNewNode(NULL, BAD_CAST "h9subscribe");
    xmlNewProp(node, BAD_CAST "event", BAD_CAST event);
    return build_h9doc(xml_length, node, xsd_validate);
}

char *h9_xmlmsg_build_h9unsubscribe(size_t *xml_length, char *event, int xsd_validate) {
    xmlNode *node = xmlNewNode(NULL, BAD_CAST "h9subscribe");
    xmlNewProp(node, BAD_CAST "event", BAD_CAST event);
    return build_h9doc(xml_length, node, xsd_validate);
}

char *h9_xmlmsg_build_h9sendmsg(size_t *xml_length, h9msg_t *msg, int xsd_validate) {
    xmlNode *node = h9msg2node(msg, "h9sendmsg");
    return build_h9doc(xml_length, node, xsd_validate);
}

char *h9_xmlmsg_build_h9msg(size_t *xml_length, h9msg_t *msg, int xsd_validate) {
    xmlNode *node = h9msg2node(msg, "h9msg");
    return build_h9doc(xml_length, node, xsd_validate);
}

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

static xmlNode *h9msg2node(h9msg_t *msg, const char *node_name) {
    xmlNode *node = xmlNewNode(NULL, BAD_CAST node_name);

    if (msg->endpoint) {
        xmlNewProp(node, BAD_CAST "endpoint", BAD_CAST msg->endpoint);
    }
    if (msg->priority == H9_MSG_PRIORITY_HIGH) {
        xmlNewProp(node, BAD_CAST "priority", BAD_CAST "H");
    }
    else {
        xmlNewProp(node, BAD_CAST "priority", BAD_CAST "L");
    }
    char str[24];
    snprintf(str, 24, "%u", (unsigned int)msg->type);
    xmlNewProp(node, BAD_CAST "type", BAD_CAST str);
    snprintf(str, 24, "%u", (unsigned int)msg->source_id);
    xmlNewProp(node, BAD_CAST "source", BAD_CAST str);
    snprintf(str, 24, "%u", (unsigned int)msg->destination_id);
    xmlNewProp(node, BAD_CAST "destination", BAD_CAST str);
    snprintf(str, 24, "%u", (unsigned int)msg->dlc);
    xmlNewProp(node, BAD_CAST "dlc", BAD_CAST str);

    for (int i = 0; i < msg->dlc; i++) {
        snprintf(&str[i*2], 24, "%02hhX", msg->data[i]);
    }
    if (msg->dlc) {
        xmlNewProp(node, BAD_CAST "data", BAD_CAST str);
    }

    return node;
}

static char *build_h9doc(size_t *xml_length, xmlNode *node, int xsd_validate) {
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlDocSetRootElement(doc, node);

    if (xsd_validate) {
        if (invalid_xml(doc)) {
            h9_log_warn("xmlmsg build: invalid XML message");
            xmlFreeDoc(doc);
            return NULL;
        }
    }

    xmlBufferPtr buffer = xmlBufferCreate();
    //*xml_length = xmlNodeDump(buffer, NULL, node, 0, 1);
    int res = xmlNodeDump(buffer, doc, node, 0, 1);
    char *ret = NULL;

    if (res > 0) {
        ret = strdup((char*) buffer->content);
        *xml_length = (size_t)res;
        h9_log_debug("xmlmsg build(%d): %s", *xml_length, ret);
    }

    xmlBufferFree(buffer);
    xmlFreeDoc(doc);

    return ret;
}
