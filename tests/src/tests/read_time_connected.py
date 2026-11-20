import asyncio
import sys
import time
from bleak import BleakScanner, BleakClient
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData

DEVICE_DISCOVERY_TIMEOUT = 5

SERVICE_UUID = "adfeb10e-d7f3-40c7-a5e1-e09ed8440203"
CHAR_UUID = "cfb51da8-8827-49b3-ba0f-40981f86f4ec"

MEASUREMENT_DURATION_S = 30

TOLERANCE_MS = 250


def advertises_status_service(device: BLEDevice, data: AdvertisementData):
    return SERVICE_UUID in data.service_uuids


async def main():
    print("Scanning for devices")

    device = await BleakScanner.find_device_by_filter(
        advertises_status_service, timeout=DEVICE_DISCOVERY_TIMEOUT
    )

    if device:
        print(f"Found {device.name}")
    else:
        print(
            f"Failed to find {SERVICE_UUID} uuid within {DEVICE_DISCOVERY_TIMEOUT} seconds.",
            file=sys.stderr,
        )
        sys.exit(1)

    async with BleakClient(device) as client:
        time_started = time.monotonic()
        first_raw_data = await client.read_gatt_char(CHAR_UUID)
        first_time = (time.monotonic() - time_started) * 1000
        print(f"READING RAWCHAR DATA: {first_raw_data}")
        print(f"HOST TIME STARTING NOW: {first_time}")

        if len(first_raw_data) == 4:
            first_data = int.from_bytes(
                first_raw_data, byteorder="little", signed=False
            )
            print(f"CONVERTING RAW DATA TO {first_data}")
        else:
            print("INCORRECT RAW DATA FAILING RUN AGAIN")
            sys.exit(1)

        print(f"WAITING {MEASUREMENT_DURATION_S} seconds")
        await asyncio.sleep(MEASUREMENT_DURATION_S)

        last_raw_data = await client.read_gatt_char(CHAR_UUID)
        last_time = (time.monotonic() - time_started) * 1000
        print(f"READING RAWCHAR DATA: {last_raw_data}")
        print(f"HOST TIME ENDING NOW: {last_time}")

        if len(last_raw_data) == 4:
            last_data = int.from_bytes(last_raw_data, byteorder="little", signed=False)
            print(f"CONVERTING RAW DATA TO {last_data}")
        else:
            print("INCORRECT RAW DATA LEN SKIPPING DATA")
            sys.exit(1)

        device_delta = last_data - first_data
        host_delta = last_time - first_time
        error = device_delta - host_delta

        print(
            f"Device Delta: {device_delta} | Host Delta: {host_delta} | Error: {error:+.1f}"
        )

        if abs(error) <= TOLERANCE_MS:
            print("PASS")
            sys.exit(0)
        else:
            print(
                f"[FAIL] Device Delta: {device_delta} | Host Delta: {host_delta} | Error: {abs(error)}",
                file=sys.stderr,
            )
            sys.exit(1)


if __name__ == "__main__":
    asyncio.run(main())
