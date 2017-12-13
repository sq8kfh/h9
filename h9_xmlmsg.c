#include "h9_xmlmsg.h"
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include "h9msg.xsd.h"

#include "h9_log.h"

int validate_xml(xmlDocPtr doc) {
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

int h9_xmlmsg_pare(char *msg, size_t msg_size, int xsd_validate) {
    xmlDocPtr doc;

    doc = xmlReadMemory(msg, msg_size, "noname.xml", NULL, 0);
    if (doc == NULL) {
        h9_log_warn("Failed to parse xml msg");
        return 0;
    }

    if (xsd_validate) {
        int res;
        if ((res = validate_xml(doc))) printf("!!invalid %d\n", res);
    }

    xmlNode *root_element = xmlDocGetRootElement(doc);

    if (root_element && root_element->type == XML_ELEMENT_NODE) {
        printf("node type: Element, name: %s\n", root_element->name);
    }

    xmlFreeDoc(doc);
    return 1;
}
