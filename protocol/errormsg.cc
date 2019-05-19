/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "errormsg.h"

ErrorMsg::ErrorMsg(GenericMsg&& k): ConcretizeMsg(std::move(k)) {
}

ErrorMsg::ErrorMsg(int errnum, std::string msg): ConcretizeMsg() {

}
