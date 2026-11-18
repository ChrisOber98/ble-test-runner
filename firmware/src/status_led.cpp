#include <Arduino.h>

#include "status_led.h"

static const int LED_PIN = 2;
static const int HEALTHY_BLINK_PERIOD_MS = 1000;
static const int BLE_ERROR_BLINK_PERIOD_MS = 100;
static const int BLE_ERROR_BLINK_COUNT = 2;

static void blink_led(int blink_ms, int blink_count = 1)
{
    for (int i = 0; i < blink_count; i++)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(blink_ms / 2);
        digitalWrite(LED_PIN, LOW);
        delay(blink_ms / 2);
    }
}

void status_led_init()
{
    pinMode(LED_PIN, OUTPUT);
}

void status_led_show_healthy()
{
    blink_led(HEALTHY_BLINK_PERIOD_MS);
}

void status_led_show_ble_error()
{
    blink_led(BLE_ERROR_BLINK_PERIOD_MS, BLE_ERROR_BLINK_COUNT);
    delay(HEALTHY_BLINK_PERIOD_MS - (BLE_ERROR_BLINK_COUNT * BLE_ERROR_BLINK_PERIOD_MS));
}
