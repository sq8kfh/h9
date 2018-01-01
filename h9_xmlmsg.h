#ifndef _H9_XMLMSG_H_
#define _H9_XMLMSG_H_

#include <stdlib.h>

#include "h9msg.h"

#define H9_XMLMSG_UNKNOWN         0
#define H9_XMLMSG_METHODCALL      1
#define H9_XMLMSG_METHODRESPONSE  2
#define H9_XMLMSG_SENDMSG         3
#define H9_XMLMSG_MSG             4
#define H9_XMLMSG_SUBSCRIBE       5
#define H9_XMLMSG_UNSUBSCRIBE     6

//typedef struct {
//    char *name;
//    char *value;
//} h9_xmlmsg_param_t;

//call always in pair with h9_xmlmsg_free_parse_data !
int h9_xmlmsg_parse(const char *msg, size_t msg_size, void **data, int xsd_validate);
void h9_xmlmsg_free_parse_data(int parse_result, void *parse_data);

char *h9_xmlmsg_build_h9methodCall(size_t *xml_length, int xsd_validate);
char *h9_xmlmsg_build_h9methodResponse(size_t *xml_length, int xsd_validate);
char *h9_xmlmsg_build_h9subscribe(size_t *xml_length, char *event, int xsd_validate);
char *h9_xmlmsg_build_h9unsubscribe(size_t *xml_length, char *event, int xsd_validate);
char *h9_xmlmsg_build_h9sendmsg(size_t *xml_length, h9msg_t *msg, int xsd_validate);
char *h9_xmlmsg_build_h9msg(size_t *xml_length, h9msg_t *msg, int xsd_validate);

#endif //_H9_XMLMSG_H_
