#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEService.h>
#include <BLECharacteristic.h>

const char *DEVICE_STATUS_SERVICE_UUID = "adfeb10e-d7f3-40c7-a5e1-e09ed8440203";
const char *TIME_CONNECTED_MS_CHARACTERISTIC_UUID = "cfb51da8-8827-49b3-ba0f-40981f86f4ec";
const int LED_PIN = 2;
const int HEALTHY_BLINK_PERIOD_MS = 1000;
const int BLE_UNINITIALIZED_BLINK_PERIOD_MS = 100;
const int BLE_UNINITIALIZED_BLINK_COUNT = 2;
const int BAUD_RATE = 115200;

int time_of_connection_ms = 0;
bool is_ble_setup = false;
bool is_device_connected = false;

void setup_ble();
void blink_led(int blink_ms, int blink_count = 1);
void log_message(const char *message);

class MyServerCallbacks : public BLEServerCallbacks
{
public:
  void onConnect(BLEServer *server) override
  {
    is_device_connected = true;
    time_of_connection_ms = millis();
    Serial.printf("[%lu] Starting connection at: %d\n", millis(), time_of_connection_ms);
    log_message("Device has connected to us.");
  }

  void onDisconnect(BLEServer *server) override
  {
    log_message("Device has disconnected from us.");
    server->startAdvertising();
    log_message("Advertising again.");
  }
};

class TimeConnectedMsCharacteristic : public BLECharacteristicCallbacks
{
public:
  void onRead(BLECharacteristic *pCharacteristic) override
  {
    int current_time = millis();
    int time_ran_ms = current_time - time_of_connection_ms;
    Serial.printf("[%lu] Setting Value of time_connected char to: %d\n", millis(), time_ran_ms);
    pCharacteristic->setValue(time_ran_ms);
  }
};

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

  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new MyServerCallbacks());
  BLEService *device_status_service = server->createService(DEVICE_STATUS_SERVICE_UUID);
  uint32_t temp_value = 0x12345678;
  BLECharacteristic *time_connected_char = device_status_service->createCharacteristic(TIME_CONNECTED_MS_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  time_connected_char->setCallbacks(new TimeConnectedMsCharacteristic());
  time_connected_char->setValue(temp_value);
  device_status_service->start();
  BLEAdvertising *advertising_object = BLEDevice::getAdvertising();
  advertising_object->addServiceUUID(DEVICE_STATUS_SERVICE_UUID);

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
