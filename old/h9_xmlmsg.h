#ifndef _H9_XMLMSG_H_
#define _H9_XMLMSG_H_

#include <stdlib.h>
#include <libxml/xmlschemas.h>

#include "h9msg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define H9_XMLMSG_UNKNOWN         0
#define H9_XMLMSG_METHODCALL      1
#define H9_XMLMSG_METHODRESPONSE  2
#define H9_XMLMSG_MSG             3
#define H9_XMLMSG_SUBSCRIBE       4
#define H9_XMLMSG_UNSUBSCRIBE     5
#define H9_XMLMSG_METRICS         6

typedef struct {
    int type;
    xmlNode *node;
} h9_xmlmsg_t;

typedef struct {
    char *name;
    char *value;
} h9_xmlmsg_metrics_t;

h9_xmlmsg_t *h9_xmlmsg_init(int type);
h9_xmlmsg_t *h9_xmlmsg_parse(const char *msg, size_t msg_size, int xsd_validate);
void h9_xmlmsg_free(h9_xmlmsg_t *xmlmsg);

char *h9_xmlmsg_to_xml(const h9_xmlmsg_t *xmlmsg, size_t *xml_length, int xsd_validate);

char *h9_xmlmsg_build_h9subscribe(size_t *xml_length, const char *event, int xsd_validate);
char *h9_xmlmsg_build_h9unsubscribe(size_t *xml_length, const char *event, int xsd_validate);
char *h9_xmlmsg_build_h9msg(size_t *xml_length, const h9msg_t *msg, int xsd_validate);

//H9_XMLMSG_MSG
h9msg_t *h9_xmlmsg_get_h9msg(const h9_xmlmsg_t *xmlms);

//H9_XMLMSG_SUBSCRIBE H9_XMLMSG_UNSUBSCRIBE
char *h9_xmlmsg_get_event(const h9_xmlmsg_t *xmlms);

//char *h9_xmlmsg_build_h9methodCall(size_t *xml_length, int xsd_validate);
//char *h9_xmlmsg_build_h9methodResponse(size_t *xml_length, int xsd_validate);

//H9_XMLMSG_METRICSES
void h9_xmlmsg_add_metrics(h9_xmlmsg_t *xmlmsg, const char *name, const char *val);
h9_xmlmsg_metrics_t *h9_xmlmsg_get_metrics_list(const h9_xmlmsg_t *xmlms);
void h9_xmlmsg_get_metrics_list_free(h9_xmlmsg_metrics_t *metrics_list);

#ifdef __cplusplus
}
#endif

#endif //_H9_XMLMSG_H_
