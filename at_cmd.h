#ifndef __AT_CMD_H
#define __AT_CMD_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "ticker.h"

#define AT_CMD_DEBUG    1

typedef size_t (*at_rx_cb)(uint8_t*, size_t);
typedef size_t (*at_tx_cb)(uint8_t*, size_t);

typedef struct
{
  char *buf;
  size_t size;
} at_buffer_t;

/**
 * @brief at_cmd_struct
 *  used for define at command
 *
 */
typedef struct at_funcation {
    char *cmd_name;                            /*!< at command name */
    int8_t cmd_len;                            /*!< at command length */
    void (*test_cmd)(struct at_funcation*);    /*!< Test Command function pointer AT+<CMD>=?\r */
    void (*query_cmd)(struct at_funcation*);   /*!< Query Command function pointer AT+<CMD>?\r */
    void (*setup_cmd)(struct at_funcation*, char *p_para); /*!< Setup Command function pointer AT+<CMD>=<data>\r */
    void (*exe_cmd)(struct at_funcation*);     /*!< Execute Command function pointer AT+<CMD>\r */
    at_tx_cb write;                            /*!< Execute write to framework output */
} at_funcation_t;

typedef struct
{
  at_rx_cb input_cb;
  at_tx_cb output_cb;
  at_funcation_t* cmd_table;
  size_t cmd_num;
  ticker_t timeout;
  size_t size;        /* Size of buffer */
  char *buff;         /* Pointer to buffer process at command */
  uint8_t at_id;
  uint8_t at_capture;
  uint8_t buff_id;
} at_cmd_cxt_t;

/* [AT_BUF_SIZE + 1] reserve NULL terminal*/
#define AT_DEVICE_INIT(P_COMM_PARAMS, AT_BUF_SIZE) \
  do                                               \
  {                                                \
    at_buffer_t buffer;                            \
    static char buf[AT_BUF_SIZE + 1];              \
    buffer.buf = buf;                              \
    buffer.size = sizeof(buf);                     \
    at_device_init(P_COMM_PARAMS, &buffer);        \
  } while (0)

extern const char at_cmd_ok[];
extern const char at_cmd_error[];

void at_device_init(at_cmd_cxt_t *at, at_buffer_t *p_buffer);

void at_cmd_capture(at_cmd_cxt_t *at);

#ifdef __cplusplus
}
#endif

#endif
