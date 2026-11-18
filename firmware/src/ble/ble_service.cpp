#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEService.h>
#include <BLECharacteristic.h>

#include "ble/ble_service.h"
#include "logger.h"

static const char *DEVICE_NAME = "Ble Test Runner";
static const char *DEVICE_STATUS_SERVICE_UUID = "adfeb10e-d7f3-40c7-a5e1-e09ed8440203";
static const char *TIME_CONNECTED_MS_CHARACTERISTIC_UUID = "cfb51da8-8827-49b3-ba0f-40981f86f4ec";
static bool is_ready = false;
static uint32_t connection_start_ms = 0;

class ServerCallbacks : public BLEServerCallbacks
{
public:
    void onConnect(BLEServer *server) override
    {
        connection_start_ms = millis();
        log_message("Device connected");
    }

    void onDisconnect(BLEServer *server) override
    {
        log_message("Device disconnected");
        server->startAdvertising();
        log_message("Advertising restarted");
    }
};

class TimeConnectedCallbacks : public BLECharacteristicCallbacks
{
public:
    void onRead(BLECharacteristic *characteristic) override
    {
        uint32_t time_connected_ms = millis() - connection_start_ms; // computed at read time
        log_message("time_connected_ms read: %lu", (unsigned long)time_connected_ms);
        characteristic->setValue(time_connected_ms);
    }
};

void setup_ble()
{
    bool init_ok = BLEDevice::init(DEVICE_NAME);
#ifdef FORCE_BLE_INIT_FAIL
    log_message("Ble init fault injected");
    init_ok = false;
#endif
    if (!init_ok)
    {
        log_message("Ble init failed");
        return;
    }

    BLEServer *server = BLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());

    BLEService *service = server->createService(DEVICE_STATUS_SERVICE_UUID);
    BLECharacteristic *time_connected = service->createCharacteristic(
        TIME_CONNECTED_MS_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
    time_connected->setCallbacks(new TimeConnectedCallbacks());
    uint32_t initial_value = 0;
    time_connected->setValue(initial_value);
    service->start();

    BLEDevice::getAdvertising()->addServiceUUID(DEVICE_STATUS_SERVICE_UUID);
    BLEDevice::startAdvertising();

    is_ready = true;
    log_message("Ble advertising started");
}

bool ble_is_ready()
{
    return is_ready;
}
