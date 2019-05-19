#ifndef _H9_ERRORMSG_H_
#define _H9_ERRORMSG_H_

#include "common/logger.h"
#include "concretizemsg.h"


class ErrorMsg: public ConcretizeMsg<GenericMsg::Type::ERROR> {
public:
    ErrorMsg(GenericMsg&& k);
    ErrorMsg(int errnum, std::string msg);
};


#endif //_H9_ERRORMSG_H_
