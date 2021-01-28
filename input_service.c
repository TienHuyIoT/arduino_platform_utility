#include "input_service.h"

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void service_io_input_process(struct io_input_cxt* io_input);

/* Private user code ---------------------------------------------------------*/
static void io_input_event_cb(struct io_input_cxt* io_input, uint8_t evt)
{
  service_io_input_handle_t *service = (service_io_input_handle_t*)io_input->handle;

  if (IO_FALLING == evt)
  {
    if (service->evt_cb)
    {
      service->evt_cb(service, IO_INPUT_FALLING);
    }
  }
  else if (IO_RISING == evt)
  {
    if (service->evt_cb)
    {
      service->evt_cb(service, IO_INPUT_RISING);
    }
  }
  else
  {
    service_io_input_process(io_input);

    /* Check timeout service idle */
    if(ticker_expried(&service->tick_idle))
    {
      ticker_begin(&service->tick_idle, service->idle_time_trigger);
      if (service->evt_cb)
      {
        service->evt_cb(service, BUTTON_IDLE_EVT);
      }
    }
  }
}

void service_io_input_init(io_input_cxt_t *io_input, service_io_input_handle_t *service)
{
  io_input->handle   = service;
  io_input->event_cb = io_input_event_cb;

  /* Init tick service idle */
  ticker_begin(&service->tick_idle, service->idle_time_trigger);
}

static void service_io_input_process(struct io_input_cxt* io_input)
{
  service_io_input_handle_t *service = (service_io_input_handle_t*)io_input->handle;

  if (service->level_active == io_input_level_get(io_input))
  {
    service->active_time = io_input_get_time(io_input, service->level_active);
    if (service->active_time > service->hold_time_trigger)
    {
      if (service->last_time_trigger < service->active_time)
      {
        service->last_time_trigger = service->active_time + service->next_time_trigger;

        service->release_count = 0;

        ticker_refresh(&service->tick_idle);

        if (service->evt_cb)
        {
          service->evt_cb(service, BUTTON_HOLD_ON_EVT);
        }
      }
    }
  }

  /* detect button is release */
  if (io_input_edge_get(io_input, service->edge_release))
  {
    ticker_refresh(&service->tick_idle);

    service->release_count++;
    if (1 == service->release_count)
    {
      if (service->active_time <= service->single_time_trigger)
      {
        /* Init tick timeout to checkout single click */
        ticker_begin(&service->tick_single_click, service->single_time_trigger);
      }
      else if (service->active_time <= service->hold_time_trigger)
      {
        /* Force single click callback */
        if (service->evt_cb)
        {
          service->evt_cb(service, BUTTON_SINGER_EVT);
        }
        service->release_count = 0;
      }
      else
      {
        service->release_count = 0;
      }
    }
    else if (2 == service->release_count)
    {
      if (service->active_time <= service->hold_time_trigger)
      {
        if (service->evt_cb)
        {
          service->evt_cb(service, BUTTON_DOUBLE_EVT);
        }
      }
      /* Stop timeout single click anyway */
      ticker_stop(&service->tick_single_click);

      service->release_count = 0;
    }
    /* reset time button hold */
    service->last_time_trigger = 0;
  }

  /* Check timeout single click */
  if(ticker_expried(&service->tick_single_click))
  {
    if (service->evt_cb)
    {
      service->evt_cb(service, BUTTON_SINGER_EVT);
    }
    service->release_count = 0;
  }
}
