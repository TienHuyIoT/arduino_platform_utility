#include <arduino.h>
#include "flatform_ticker.h"
#include "frame.h"
#include "frame_com.h"
#include "if_frame_app.h"

#if (defined IF_COM_DEBUG_ENABLE) && (IF_COM_DEBUG_ENABLE == 1)
#include "console_dbg.h"
#define EVSE_TAG_PRINTF(...) CONSOLE_TAG_LOGI("[EVSE]", __VA_ARGS__)
#define EVSE_PRINTF(...) CONSOLE_LOGI(__VA_ARGS__)
#else
#define EVSE_PRINTF(f_, ...)
#define EVSE_TAG_PRINTF(f_, ...)
#endif

#define FRAME_IF_BYTE_ACK	  0
#define FRAME_IF_BYTE_NACK	1

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  int command;
  void (*process_cb)(frame_com_cxt_t*, if_command_t, uint8_t*, size_t);
} if_cmd_handle_t;

/* Private define ------------------------------------------------------------*/
#define IF_CMD_HANDLE_NUM  (sizeof(if_cmd_handle) / sizeof(if_cmd_handle[0]))

/* Private variables ---------------------------------------------------------*/
/**@brief String literals for the serial command. */
const char* const lit_command_id[17] PROGMEM = {
/* 0 */ "FRAME_IF_HEART_BEAT",
/* 1 */ "FRAME_IF_RTC",
/* 2 */ "FRAME_IF_ERR_TYPE",
/* 3 */ "FRAME_IF_HW_VERSION",
/* 4 */ "FRAME_IF_FW_VERSION",
/* 5 */ "NC0",
/* 6 */ "NC1",
/* 7 */ "NC2",
/* 8 */ "NC3",
/* 9 */ "NC4",
/* 10*/ "NC5",
/* 11*/ "NC6",
/* 12*/ "NC7",
/* 13*/ "NC8",
/* 14*/ "NC9",
/* 15*/ "NC10",
/* 16*/ "FRAME_IF_JIG_TEST"
};

const char* const lit_frame_result_id[6] PROGMEM = {
  "FRAME_OK",
  "FRAME_ERR",
  "FRAME_SIZE_MIN_ERR",
  "FRAME_SOF_EOF_ERR",
  "FRAME_LENGTH_PACK_ERR",
  "FRAME_CRC_ERR"
};

/* Private function prototypes -----------------------------------------------*/

static if_callback_handle_t *_callback;

static void make_ack_response(frame_com_cxt_t *fc, if_command_t cmd);
static void make_nack_response(frame_com_cxt_t *fc, if_command_t cmd);
static void heart_beat_cmd_receive(frame_com_cxt_t *fc, if_command_t cmd,
    uint8_t *data, size_t length);
static void jig_test_mode_recieve(frame_com_cxt_t *fc, if_command_t cmd,
    uint8_t *data, size_t length);

static if_cmd_handle_t const if_cmd_handle[] = { 
  { FRAME_IF_HEART_BEAT, heart_beat_cmd_receive }, 
  { FRAME_IF_ERR_TYPE  , NULL }, 
  { FRAME_IF_HW_VERSION, NULL }, 
  { FRAME_IF_FW_VERSION, NULL }, 
  { FRAME_IF_JIG_TEST  , jig_test_mode_recieve } 
};

void if_callback_register(if_callback_handle_t *cb)
{
  _callback = cb;
}

/* Brief: API send frame command to EVSE
 * [cmd]: command refer from rfid_command_t
 * [data]: data buffer shall fill to data's field of frame command
 * [length]: the length of data buffer
 * */
void if_frame_transmit(frame_com_cxt_t *fc, uint8_t cmd, uint8_t *data,
    uint16_t length)
{
  if (FRAME_IF_NUM > cmd) {
    EVSE_PRINTF("\r\n[%u][ Send frame ]", fc->instance);
    EVSE_PRINTF("\r\n- CMD: %s", FPSTR(lit_command_id[cmd]));
    EVSE_PRINTF("\r\n- Data length: %u", length);EVSE_PRINTF("\r\n- Data: ");
    if (0 != length) {
      for (uint16_t i = 0; i < length; ++i) {
        EVSE_PRINTF("{%02X} ", data[i]);
      }
    }
    else {
      EVSE_PRINTF("NONE");
    }EVSE_PRINTF("\r\n");
    frame_com_transmit(fc, cmd, data, length);
  }
  else {
    EVSE_PRINTF("\r\n[ERROR] Unknown Command ");
  }
}

/* Brief: the function event callback parsed a frame message */
void if_receive_cmd_callback(frame_com_cxt_t *fc, uint8_t result, uint8_t cmd,
    uint8_t *data, uint16_t length)
{
  EVSE_PRINTF("\r\n[%u] Result Receive command: %s", fc->instance, FPSTR(lit_frame_result_id[result]));
  
  if ((uint8_t) FRAME_OK == result) {
    if (FRAME_IF_NUM > cmd) {
      if_cmd_handle_t *p_cmd = (if_cmd_handle_t*) if_cmd_handle;
      uint8_t cmd_handle = 0;
      
      EVSE_PRINTF("\r\n[%u][ Get new frame ]", fc->instance); 
      EVSE_PRINTF("\r\n- CMD: %s", FPSTR(lit_command_id[cmd])); 
      EVSE_PRINTF("\r\n- Data length: %u", length);
	    EVSE_PRINTF("\r\n- Data: ");
      for (uint16_t i = 0; i < length; ++i) {
        EVSE_PRINTF("{%02X} ", data[i]);
      }EVSE_PRINTF("\r\n");
      
      for (uint8_t i = 0; i < IF_CMD_HANDLE_NUM; ++i) {
        /* Find command handle */
        if (p_cmd[i].command == cmd) {
          /* Assert callback function */
          if (p_cmd[i].process_cb) {
            p_cmd[i].process_cb(fc, (if_command_t)cmd, data, length);
          }
          else {
            EVSE_PRINTF("\r\nNone callback function handle");
          }
          
          cmd_handle = 1;
          break;
        } // if (p_cmd[i].command == cmd)
      } // For()
      
      if (!cmd_handle) {
        EVSE_PRINTF("\r\nCommand[%u] handle None", cmd);
      }
    }
    else {
      EVSE_PRINTF("\r\n[%u] Unknown Command\r\n", fc->instance);
    }
  } // if((uint8_t)FRAME_OK == result)
}

static void make_ack_response(frame_com_cxt_t *fc, if_command_t cmd)
{
  uint8_t data_buf[ACK_DF_LENGTH];
  data_buf[ACK_DF_ACK_INDEX] = FRAME_IF_BYTE_ACK;
  if_frame_transmit(fc, (uint8_t) cmd, data_buf, ACK_DF_LENGTH);
  EVSE_PRINTF("\r\n[%u][%s] ACK response", fc->instance, FPSTR(lit_command_id[cmd]));
  EVSE_PRINTF("\r\n");
}

static void make_nack_response(frame_com_cxt_t *fc, if_command_t cmd)
{
  uint8_t data_buf[ACK_DF_LENGTH];
  data_buf[ACK_DF_ACK_INDEX] = FRAME_IF_BYTE_NACK;
  if_frame_transmit(fc, (uint8_t) cmd, data_buf, ACK_DF_LENGTH);
  EVSE_PRINTF("\r\n[%u][%s] NACK response", fc->instance, FPSTR(lit_command_id[cmd]));
  EVSE_PRINTF("\r\n");
}

/* RFID Handle receive heart beat command
 * Response ACK/NACK
 * HEART BEAT command
 * Hercules terminal test string: $7E$02$00$02$7F
 * Hex recieve:      {7E}{02}{00}{02}{7F}
 * Hex ACK response: {7E}{03}{00}{00}{03}{7F}
 * */
static void heart_beat_cmd_receive(frame_com_cxt_t *fc, if_command_t cmd,
    uint8_t *data, size_t length)
{
  /* Assert data length heart beat receive command */
  if (0 == length) {
    EVSE_PRINTF("\r\n[%u][%s] command succeed", fc->instance, FPSTR(lit_command_id[cmd]));
    make_ack_response(fc, cmd);
  }
  else {
    EVSE_PRINTF("\r\n[%u][%s] length command failure", fc->instance, FPSTR(lit_command_id[cmd]));
    make_nack_response(fc, cmd);
  }EVSE_PRINTF("\r\n");
}

/* Command enter jig mode
 * Receive :$7E$06$10$01$02$03$04$12$7F
 * Response:$7E$03$10$00$13$7F
 * */
static void jig_test_mode_recieve(frame_com_cxt_t *fc, if_command_t cmd,
    uint8_t *data, size_t length)
{
  uint8_t jig_data[4] = { 0x01, 0x02, 0x03, 0x04 };
  
  /* assert length */
  if (JIG_DF_LENGTH == length) {
    /* assert data */
    if (!memcmp(data, jig_data, JIG_DF_LENGTH)) {
      EVSE_PRINTF("\r\n[%u][%s] command succeed", fc->instance, FPSTR(lit_command_id[cmd]));
      make_ack_response(fc, cmd);
      if (_callback->jig_setup_cb) {
        /* Jig enable */
        _callback->jig_setup_cb(1);
      }
    }
    else {
      EVSE_PRINTF("\r\n[%u][%s] data command failure", fc->instance, FPSTR(lit_command_id[cmd]));
      make_nack_response(fc, cmd);
    }
  }
  else {
    EVSE_PRINTF("\r\n[%u][%s] length command failure", fc->instance, FPSTR(lit_command_id[cmd]));
    make_nack_response(fc, cmd);
  }
}
