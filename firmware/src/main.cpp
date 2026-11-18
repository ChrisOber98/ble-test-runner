#include <Arduino.h>

#include "ble/ble_service.h"
#include "logger.h"
#include "status_led.h"

static const int BAUD_RATE = 115200;

void setup()
{
  Serial.begin(BAUD_RATE);
  status_led_init();
  setup_ble();
}

void loop()
{
#ifdef FORCE_LOGGING_TRUNCATION
  char buffer[151]; // 150 characters + 1 for the '\0' terminator

  for (int i = 0; i < 150; i++)
    buffer[i] = '!';

  buffer[150] = '\0';

  log_message("%s", buffer);
  delay(1000);
  return;
#endif
  if (ble_is_ready())
  {
    status_led_show_healthy();
    log_message("Alive");
  }
  else
  {
    status_led_show_ble_error();
    log_message("Ble Uninitialized");
  }
}
