#include "flatform_ticker.h"
#include "frame.h"
#include "frame_com.h"
#include "if_frame_app.h"
#include "io_input.h"
#include "input_service.h"
#include "at_cmd.h"
#include "at_cmd_process.h"
#include "console_dbg.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
#define EVSE_FC_RX_BUFF_SIZE    FRAME_SIZE_MAX
#define EVSE_FC_TX_BUFF_SIZE    FRAME_SIZE_MAX

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define MAIN_TAG_CONSOLE(...) CONSOLE_TAG_LOGI("[MAIN]", __VA_ARGS__)
#define MAIN_CONSOLE(...) CONSOLE_LOGI(__VA_ARGS__)

/* Private variables ---------------------------------------------------------*/
const char* const button_serviceid[] PROGMEM = {
  /* 0 */"IO_INPUT_FALLING",
  /* 1 */"IO_INPUT_RISING",
  /* 2 */"BUTTON_SINGER_EVT",
  /* 3 */"BUTTON_DOUBLE_EVT",
  /* 4 */"BUTTON_HOLD_ON_EVT",
  /* 5 */"BUTTON_IDLE_EVT"
};
static frame_com_cxt_t frame_com_if_uart;
static if_callback_handle_t if_callback_handle;
static at_cmd_cxt_t at_cmd_cxt_uart;
static at_cmd_callback_handle_t at_cmd_callback_handle;

static io_input_cxt_t button_cxt = IO_INPUT_DEFAULT;
static service_io_input_handle_t button_service = SERVICE_IO_INPUT_DEFAULT;
static ticker_function_handle_t tick_button_sample;

static uint8_t jig_process_enable = 0;

uint32_t write_uart_instance(uint8_t *s, uint32_t len)
{
  return Serial.write(s, len);
}

uint32_t read_uart_instance(uint8_t *s, uint32_t len)
{
  return Serial.read(s, len);
}

/*  */
void button_sample_rate_cb(uint32_t remain) {
  uint8_t ip_status;
  (void) remain;

  ip_status = digitalRead(0);
  io_input_process(&button_cxt, ip_status);
}

void button_service_cb(service_io_input_handle_t *service, uint8_t evt) {
  MAIN_TAG_CONSOLE("Button service: %s", FPSTR(button_serviceid[evt]));
  MAIN_CONSOLE("\r\n");
  if (service == &button_service) {
    switch (evt) {
      case IO_INPUT_FALLING:
        break;

      case IO_INPUT_RISING:
        break;

      case BUTTON_SINGER_EVT:
        break;

      case BUTTON_DOUBLE_EVT:
        break;

      case BUTTON_HOLD_ON_EVT:
        break;

      case BUTTON_IDLE_EVT:
        break;

      default:
        break;
    }
  }
}

uint8_t jig_query(void)
{
  return (1 == jig_process_enable);
}

void jig_setup(uint8_t jig)
{
  jig_process_enable = jig;
  if (jig)
  {
    MAIN_TAG_CONSOLE("JIG Enable");
  }
  else
  {
    MAIN_TAG_CONSOLE("JIG Disable");
  }

  MAIN_TAG_CONSOLE("");
}

void setup() {
  CONSOLE_PORT.begin(115200);
  pinMode(0, INPUT_PULLUP);
  /* Register serial interface with rfid */
  frame_com_if_uart.instance = FRAME_UART_INTERFACE;
  frame_com_if_uart.input_cb = read_uart_instance;
  frame_com_if_uart.output_cb = write_uart_instance;
  /* Init frame and process receive command callback function */
  FRAME_COM_INIT(&frame_com_if_uart, if_receive_cmd_callback,
                 EVSE_FC_RX_BUFF_SIZE, EVSE_FC_TX_BUFF_SIZE);

  if_callback_handle.jig_query_cb = jig_query;  /* Register jig query */
  if_callback_handle.jig_setup_cb = jig_setup;  /* Register jig setup */
  if_callback_register(&if_callback_handle);

  /* Init at command process */
  at_cmd_cxt_uart.input_cb = read_uart_instance;
  at_cmd_cxt_uart.output_cb = write_uart_instance;
  at_cmd_cxt_uart.cmd_table = at_fun_handle;
  at_cmd_cxt_uart.cmd_num = AT_CMD_HANDLE_NUM;
  /* Init uart at command with 32 bytes buffer */
  AT_DEVICE_INIT(&at_cmd_cxt_uart, 32);

  at_cmd_callback_handle.jig_query_cb = jig_query;  /* Register jig query */
  at_cmd_callback_handle.jig_setup_cb = jig_setup;  /* Register jig setup */
  at_cmd_callback_register(&at_cmd_callback_handle);

  button_service.evt_cb = button_service_cb;
  button_service.edge_release = IO_RISING;
  button_service.level_active = IO_LOW;
  io_input_init(&button_cxt, 5, 5); /* 5 is high and low sample rate count*/
  service_io_input_init(&button_cxt, &button_service);
  /* Init tick sample rate 10ms */
  ticker_function_init(&tick_button_sample, button_sample_rate_cb, 10,
                       TICKER_FOREVER);
}

void loop() {
  if (!jig_process_enable)
  {
    frame_com_process(&frame_com_if_uart);
  }
  else
  {
    /* JIG AT command interface */
    at_cmd_capture(&at_cmd_cxt_uart);
  }

  /* timer ticker process handler */
  ticker_loop();
}
