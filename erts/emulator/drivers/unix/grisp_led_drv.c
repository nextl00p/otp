/* spi_drv.c */

#include <assert.h>

#include "erl_driver.h"
#include "sys.h"
#include <grisp/led.h>

int grisp_led_init (void);
ErlDrvData grisp_led_start (ErlDrvPort port, char *command);
void grisp_led_stop (ErlDrvData drv_data);
void grisp_led_output (ErlDrvData drv_data, char *buf, ErlDrvSizeT len);

ErlDrvEntry grisp_led_driver_entry = {
  grisp_led_init,
  grisp_led_start,
  grisp_led_stop,
  grisp_led_output,
  NULL,
  NULL,
  "grisp_led_drv",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  ERL_DRV_EXTENDED_MARKER,
  ERL_DRV_EXTENDED_MAJOR_VERSION,
  ERL_DRV_EXTENDED_MINOR_VERSION,
  0,
  NULL,
  NULL,
  NULL
};

struct grisp_led_data {
  ErlDrvPort port;
  char cnt;
};

/* FIXME: consider making this more dynamic, especially if generalized */

static struct grisp_led_data grisp_led_data = { NULL, 0 };

int grisp_led_init (void)
{
  grisp_led_set1(false, false, false);
  grisp_led_set2(false, false, false);

  return 0;
}

ErlDrvData grisp_led_start (ErlDrvPort port, char *command)
{
  if (grisp_led_data.port != NULL)
    return ERL_DRV_ERROR_GENERAL;

  grisp_led_data.port = port;
  grisp_led_data.cnt = 1;
  return (ErlDrvData)&grisp_led_data;
}

void grisp_led_stop (ErlDrvData drv_data)
{
  ASSERT ((struct grisp_led_data *)drv_data == &grisp_led_data);
  grisp_led_data.port = NULL;
}

void grisp_led_output (ErlDrvData drv_data, char *buf, ErlDrvSizeT len)
{
  unsigned char *p;
  int led_nr, r, g, b;
  
  ASSERT ((struct grisp_led_data *)drv_data == &grisp_led_data);
  ASSERT (grisp_led_data.port != NULL);

  grisp_led_data.cnt++;
  p = (unsigned char *)buf;

  led_nr = *p++;
  r = *p++;
  g = *p++;
  b = *p++;

  if (led_nr == 1)
    grisp_led_set1(r, g, b);
  else if (led_nr == 2)
    grisp_led_set2(r, g, b);
}
