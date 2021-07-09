#ifndef _INPUT_SERVICE_H
#define _INPUT_SERVICE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "io_input.h"
#include "flatform_ticker.h"

#define INPUT_SINGLE_TIME_TRIGGER     400   /* ms */
#define INPUT_HOLD_TIME_TRIGGER       1000  /* ms */
#define INPUT_NEXT_TIME_TRIGGER       50    /* ms */
#define INPUT_IDLE_TIME_TRIGGER       60000 /* ms */

#define SERVICE_IO_INPUT_DEFAULT {0,                         \
                                  0,                         \
                                  INPUT_SINGLE_TIME_TRIGGER, \
                                  INPUT_HOLD_TIME_TRIGGER,   \
                                  INPUT_NEXT_TIME_TRIGGER,   \
                                  0,                         \
                                  INPUT_IDLE_TIME_TRIGGER,   \
                                  IO_LOW,                    \
                                  IO_RISING,                 \
                                  0                          \
                                  }

typedef enum {
  IO_INPUT_FALLING,
  IO_INPUT_RISING,
  BUTTON_SINGER_EVT,
  BUTTON_DOUBLE_EVT,
  BUTTON_HOLD_ON_EVT,
  BUTTON_IDLE_EVT
} service_io_input_evt_t;

struct service_io_input_handle;

typedef struct service_io_input_handle
{
  void     (*evt_cb)(struct service_io_input_handle*, uint8_t evt);
  uint32_t active_time;          /* time input level stable */
  uint32_t single_time_trigger;  /* time trigger input level */
  uint32_t hold_time_trigger;    /* time trigger input hold */
  uint32_t next_time_trigger;    /* time trigger next event input hold  */
  uint32_t last_time_trigger;    /* time trigger last event input */
  uint32_t idle_time_trigger;    /* time trigger input idle */
  uint8_t  level_active;         /* input level active */
  uint8_t  edge_release;         /* input edge release */
  uint8_t  release_count;        /* input release counter */
  ticker_t tick_single_click;    /* tick handle double click service */
  ticker_t tick_idle;            /* tick handle idle service */
} service_io_input_handle_t;

void service_io_input_init(io_input_cxt_t *io_input, service_io_input_handle_t *service_instance);

#ifdef __cplusplus
}
#endif

#endif
