#ifndef __AT_CMD_PROCESS_H
#define __AT_CMD_PROCESS_H

#define AT_CMD_PROCESS_DEBUG 1

#include <stdint.h>
#include "at_cmd.h"

typedef struct {
  uint8_t (*jig_query_cb)(void);
  void (*jig_setup_cb)(uint8_t);
} at_cmd_callback_handle_t;

extern const size_t AT_CMD_HANDLE_NUM;

extern at_funcation_t at_fun_handle[];

void at_cmd_callback_register(at_cmd_callback_handle_t* cb);

#endif
