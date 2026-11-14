#include <Arduino.h>
#include <BLEDevice.h>

const int LED_PIN = 2;
const int HEALTHY_BLINK_PERIOD_MS = 1000;
const int BLE_UNINITIALIZED_BLINK_PERIOD_MS = 100;
const int BLE_UNINITIALIZED_BLINK_COUNT = 2;
const int BAUD_RATE = 115200;

bool is_ble_setup = false;

void setup_ble();
void blink_led(int blink_ms, int blink_count = 1);
void log_message(const char *message);

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(BAUD_RATE);
  setup_ble();
}

void loop()
{
  if (is_ble_setup)
  {
    blink_led(HEALTHY_BLINK_PERIOD_MS);
    log_message("Alive");
  }
  else
  {
    blink_led(BLE_UNINITIALIZED_BLINK_PERIOD_MS, BLE_UNINITIALIZED_BLINK_COUNT);
    delay(HEALTHY_BLINK_PERIOD_MS - (BLE_UNINITIALIZED_BLINK_COUNT * BLE_UNINITIALIZED_BLINK_PERIOD_MS));
    log_message("Ble Uninitialized");
  }
}

void setup_ble()
{
  is_ble_setup = BLEDevice::init("Ble Test Runner");
#ifdef FORCE_BLE_INIT_FAIL
  log_message("Ble init fault injected");
  is_ble_setup = false;
#endif
  if (!is_ble_setup)
  {
    log_message("Ble Init Failed");
    return;
  }

  BLEDevice::startAdvertising();
  log_message("Ble Started Advertising");
}

void blink_led(int blink_ms, int blink_count)
{
  for (int i = 0; i < blink_count; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(blink_ms / 2);
    digitalWrite(LED_PIN, LOW);
    delay(blink_ms / 2);
  }
}

void log_message(const char *message)
{
  Serial.printf("[%lu] %s\n", millis(), message);
}
