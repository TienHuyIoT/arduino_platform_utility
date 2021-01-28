#ifndef __IF_COM_APP_H
#define __IF_COM_APP_H

#include <stdint.h>
#include "frame_com.h"

#define IF_COM_DEBUG_ENABLE    1

typedef enum
{
  FRAME_UART_INTERFACE = 0,
  FRAME_RS232_INTERFACE
} if_frame_instance_t;

typedef enum
{
/* 0 */ FRAME_IF_HEART_BEAT = 0,
/* 1 */ FRAME_IF_RTC,
/* 2 */ FRAME_IF_ERR_TYPE,
/* 3 */ FRAME_IF_HW_VERSION,
/* 4 */ FRAME_IF_FW_VERSION,
/* 5 */ NC0,
/* 6 */ NC1,
/* 7 */ NC2,
/* 8 */ NC3,
/* 9 */ NC4,
/* 10*/ NC5,
/* 11*/ NC6,
/* 12*/ NC7,
/* 13*/ NC8,
/* 14*/ NC9,
/* 15*/ NC10,
/* 16*/ FRAME_IF_JIG_TEST,
/* 17*/ FRAME_IF_NUM
} if_command_t;

/* Uid data frame element index*/
#define JIG_DF_LENGTH               4 /* number of data field */

/* ACK data frame element*/
#define ACK_DF_LENGTH               1 /* number of data field */
#define ACK_DF_ACK_INDEX            0  /* length is 1 byte */

typedef struct {
  uint8_t (*jig_query_cb)(void);
  void (*jig_setup_cb)(uint8_t);
} if_callback_handle_t;

void if_callback_register(if_callback_handle_t*);
void if_frame_transmit(frame_com_cxt_t* fc, uint8_t cmd, uint8_t* data, uint16_t length);
void if_receive_cmd_callback(frame_com_cxt_t* frame_instance, uint8_t result, uint8_t cmd, uint8_t* data, uint16_t length);

#endif
