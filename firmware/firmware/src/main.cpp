#include <Arduino.h>

const int LED_PIN = 2;
const int BLINK_HALF_PEROID_MS = 500;
const int BAUD_RATE = 115200;

int blink_counter = 0;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(BAUD_RATE);
}

void loop()
{
  digitalWrite(LED_PIN, HIGH);
  delay(BLINK_HALF_PEROID_MS);
  digitalWrite(LED_PIN, LOW);
  delay(BLINK_HALF_PEROID_MS);
  blink_counter++;
  Serial.printf("Alive: %d.\n", blink_counter);
}
