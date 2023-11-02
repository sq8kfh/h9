/*
 * Created by crowx on 02/11/2023.
 *
 */

#pragma once

#include <confuse.h>

namespace confuse_helpers {

void cfg_err_func(cfg_t* cfg, const char* fmt, va_list args);
int validate_node_id(cfg_t* cfg, cfg_opt_t* opt);
int validate_node_register_type(cfg_t* cfg, cfg_opt_t* opt);
int validate_node_register_size(cfg_t* cfg, cfg_opt_t* opt);
int validate_node_type_sec(cfg_t* cfg, cfg_opt_t* opt);
int validate_node_register_number_sec(cfg_t* cfg, cfg_opt_t* opt);

} // namespace confuse_helpers
