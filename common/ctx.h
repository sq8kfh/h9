#ifndef _H9_CTX_H_
#define _H9_CTX_H_

#include <string>

#include "config.h"

class Ctx {
protected:
    unsigned int _verbose;
#ifdef DEBUG
    bool _debug;
#endif
    const std::string _app_name;
    const std::string _app_desc;
    void raise_verbose_level(unsigned int how_much);
public:
    Ctx(const std::string& app_name, const std::string& app_desc);
    inline unsigned int verbose_level();
    inline bool debug_enabled();
};


#endif //_H9_CTX_H_
