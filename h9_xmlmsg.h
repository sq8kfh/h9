#ifndef _H9_XMLMSG_H_
#define _H9_XMLMSG_H_

#include <stdlib.h>

#include "h9msg.h"

#define H9_XMLMSG_UNKNOWN           0
#define H9_XMLMSG_METHODCALL        1
#define H9_XMLMSG_H9METHODRESPONSE  2
#define H9_XMLMSG_H9SENDMSG         3
#define H9_XMLMSG_H9MSG             4

//typedef struct {
//    char *name;
//    char *value;
//} h9_xmlmsg_param_t;

int h9_xmlmsg_parse(const char *msg, size_t msg_size, int xsd_validate, void **params);

#endif //_H9_XMLMSG_H_
