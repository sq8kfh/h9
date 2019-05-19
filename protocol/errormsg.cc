#include "errormsg.h"

ErrorMsg::ErrorMsg(GenericMsg&& k): ConcretizeMsg(std::move(k)) {
}

ErrorMsg::ErrorMsg(int errnum, std::string msg): ConcretizeMsg() {

}
