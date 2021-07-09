#include <arduino.h>
#include <stdio.h>
#include "app_config.h"
#include "Tools.h"
#include "at_cmd_process.h"

#if (defined AT_CMD_PROCESS_DEBUG) && (AT_CMD_PROCESS_DEBUG == 1)
#include "console_dbg.h"
#define APP_AT_TAG_PRINT(...) CONSOLE_TAG_LOGI("[APP AT]", __VA_ARGS__)
#define APP_AT_PRINT(...) CONSOLE_LOGI(__VA_ARGS__)
#else
#define APP_AT_PRINT(f_, ...)
#define APP_AT_DBG_PRINT(f_, ...)
#endif

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define AT_DEVICE_RESP(W, f_, ...)    vspfunc(W, (f_), ##__VA_ARGS__)

/* Private variables ---------------------------------------------------------*/
static at_cmd_callback_handle_t* _callback;

/* Private function prototypes -----------------------------------------------*/
static void at_exe_cmd_null(at_funcation_t* at);
static void at_query_swv(at_funcation_t* at);
static void at_query_hwv(at_funcation_t* at);
static void at_query_sn(at_funcation_t* at);
static void at_query_jig(at_funcation_t* at);
static void at_setup_jig(at_funcation_t* at, char *p_para);

/* cmd_name, cmd_len, (*test_cmd), (*query_cmd), (*setup_cmd), (*exe_cmd)
 *         ,        , AT+<CMD>=?\r, AT+<CMD>?\r, AT+<CMD>=..\r, AT+<CMD>\r */
at_funcation_t at_fun_handle[]={
  {NULL     , 0, NULL, NULL         , NULL        , at_exe_cmd_null},
  {(char*)"+SWV"   , 4, NULL, at_query_swv , NULL        , NULL},
  {(char*)"+HWV"   , 4, NULL, at_query_hwv , NULL        , NULL},
  {(char*)"+SN"    , 3, NULL, at_query_sn  , NULL        , NULL},
  {(char*)"+JIG"   , 4, NULL, at_query_jig , at_setup_jig, NULL}
};

const size_t AT_CMD_HANDLE_NUM = (sizeof(at_fun_handle) / sizeof(at_fun_handle[0]));

void at_cmd_callback_register(at_cmd_callback_handle_t* cb)
{
  _callback = cb;
}

static void at_exe_cmd_null(at_funcation_t* at)
{
  AT_DEVICE_RESP(at->write, at_cmd_ok);
}

/* \r\n+SWV:<SPACE><major.minor.sub_minor>\r\n */
static void at_query_swv(at_funcation_t* at)
{
  AT_DEVICE_RESP(at->write, "\r\n%s: \"%u.%u.%u\"\r\n%s", at->cmd_name,
      FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_BUILD, at_cmd_ok);
}

/* \r\n+HWV:<SPACE><version string>\r\n */
static void at_query_hwv(at_funcation_t* at)
{
  AT_DEVICE_RESP(at->write, "\r\n%s: \"%s\"\r\n%s", at->cmd_name,
      (const char*)HW_VERSION_STRING, at_cmd_ok);
}

/* \r\n+SN:<SPACE><serial number string>\r\n */
static void at_query_sn(at_funcation_t* at)
{
  uint8_t sn_buff[SERIAL_NUMBER_LENGTH_MAX] = {0};

  memcpy(sn_buff, (const uint8_t*)SERIAL_NUMBER_ADDR, SERIAL_NUMBER_LENGTH_MAX - 1);
  AT_DEVICE_RESP(at->write, "\r\n%s: \"%s\"\r\n%s", at->cmd_name,
      (char*)sn_buff, at_cmd_ok);
}

/* \r\n+JIG:<SPACE><result>\r\n */
static void at_query_jig(at_funcation_t* at)
{
  if (_callback->jig_query_cb)
  {
    AT_DEVICE_RESP(at->write, "\r\n%s: %u\r\n%s", at->cmd_name,
        _callback->jig_query_cb(), at_cmd_ok);
  }
  else
  {
    AT_DEVICE_RESP(at->write, at_cmd_error);
  }
}

/* \r\n+JIG:<SPACE><result>\r\n */
static void at_setup_jig(at_funcation_t* at, char *p_para)
{
  if (1 == strlen(p_para) && '0' == p_para[0])
  {
    AT_DEVICE_RESP(at->write, "\r\n%s: 1\r\n%s", at->cmd_name, at_cmd_ok);
    if (_callback->jig_setup_cb)
    {
      /* Jig disable */
      _callback->jig_setup_cb(0);
    }
  }
  else
  {
    AT_DEVICE_RESP(at->write, "\r\n%s: 0\r\n%s", at->cmd_name, at_cmd_error);
  }
}
