# GATT services

## device status Service

### Goal
The goal of this service is to represent and track how many milliseconds have passed since the ble connection has been established.

### Service
- Name: device_status
- UUID: adfeb10e-d7f3-40c7-a5e1-e09ed8440203

### Characteristic
- Name: time_connected_ms
- UUID: cfb51da8-8827-49b3-ba0f-40981f86f4ec
- Properties: read
- Data format: binary uint32, little-endian
    - Choosing binary becuause I want to send as close to the real representation of the data we are exsposing so binrary to me for a number based value makes most sense.
- Value is computed at read time (onRead), so reads are always current.

### Connection behavior
- characteristic time_connected_ms
    - After connection
        - Internal counter for time elapaed is set start tracking time.
        - On inital start the counter will hold 0.
    - After Disconnect
        - Internal counter for time elapaed is set nack to 0.
        - Ble is set to start advertising again.

### How I'll verify it
Tolerance: TBD. Determine by measuring 20 read pairs; set to ~2× the worst observed error.

- Manually
    - Open NRF confirm device is advertising and start timer
    - Connect to device
    - read and record the characteristic value and wait 2000ms
    - read and record the characterstic value again.
    - Verify the 2 times are 2000ms apart (+ tolerance).

- automatically (Python)
    - Search for device
    - Connect to the device
    - read and record the characteristic value and wait 2000ms
    - read and record the characterstic value again.
    - Verify the 2 times are 2000ms apart (+ tolerance).

- Test: value resets after reconnect
    - Connect, wait 3 s, read → expect ≈ 3000 ms
    - Disconnect, reconnect, read immediately
    - Expect: value is small (< 1000 ms) and much less than the first reading

- Test: device advertises after reconnect
    - Connect, wait 3 s
    - Disconnect, scan
    - Expect: local_name Ble Test Runner is found again in 5000 ms.

### Limitations
4,294,967,295 is the biggest millisecond representation the device holds (1 Months 2 Weeks 5 Days 6 Hours 57 Minutes 59 Seconds).

### Open Questions
“What happens if two centrals connect at the same time?” (The ESP32 can allow it.)

