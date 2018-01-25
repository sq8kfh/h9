#ifndef _H9_XMLMSG_H_
#define _H9_XMLMSG_H_

#include <stdlib.h>
#include <libxml/xmlschemas.h>

#include "h9msg.h"

#define H9_XMLMSG_UNKNOWN         0
#define H9_XMLMSG_METHODCALL      1
#define H9_XMLMSG_METHODRESPONSE  2
#define H9_XMLMSG_SENDMSG         3
#define H9_XMLMSG_MSG             4
#define H9_XMLMSG_SUBSCRIBE       5
#define H9_XMLMSG_UNSUBSCRIBE     6
#define H9_XMLMSG_METRICSES       7

typedef struct {
    int type;
    xmlNode *node;
} h9_xmlmsg_t;

//call always in pair with h9_xmlmsg_free_parse_data !
int h9_xmlmsg_parse(const char *msg, size_t msg_size, void **data, int xsd_validate);
void h9_xmlmsg_free_parse_data(int parse_result, void *parse_data);

h9_xmlmsg_t *h9_xmlmsg_init(int type);
void h9_xmlmsg_free(h9_xmlmsg_t *xmlmsg);
char *h9_xmlmsg_build(h9_xmlmsg_t *xmlmsg, size_t *xml_length, int xsd_validate);

//char *h9_xmlmsg_build_h9methodCall(size_t *xml_length, int xsd_validate);
//char *h9_xmlmsg_build_h9methodResponse(size_t *xml_length, int xsd_validate);

char *h9_xmlmsg_build_h9subscribe(size_t *xml_length, char *event, int xsd_validate);
char *h9_xmlmsg_build_h9unsubscribe(size_t *xml_length, char *event, int xsd_validate);
char *h9_xmlmsg_build_h9sendmsg(size_t *xml_length, h9msg_t *msg, int xsd_validate);
char *h9_xmlmsg_build_h9msg(size_t *xml_length, h9msg_t *msg, int xsd_validate);

//H9_XMLMSG_METRICSES
void h9_xmlmsg_add_metrics(h9_xmlmsg_t *xmlmsg, char *name, char *val);

#endif //_H9_XMLMSG_H_
