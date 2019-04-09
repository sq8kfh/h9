#ifndef _H9D_CFG_H_
#define _H9D_CFG_H_

#define h9d_cfg_true 1
#define h9d_cfg_false 0

void h9d_cfg_init(const char *cfg_file);

void h9d_cfg_setbool(const char *name, unsigned int value);
void h9d_cfg_setint(const char *name, int value);
void h9d_cfg_setstr(const char *name, const char *value);

unsigned int h9d_cfg_getbool(const char *name);
int h9d_cfg_getint(const char *name);
char *h9d_cfg_getstr(const char *name);

const char *h9d_cfg_endpoint(void);
unsigned int h9d_cfg_endpoint_getbool(const char *name);
int h9d_cfg_endpoint_getint(const char *name);
char *h9d_cfg_endpoint_getstr(const char *name);

#endif //_H9D_CFG_H_
