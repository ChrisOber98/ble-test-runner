import asyncio
import sys
from bleak import BleakScanner

DEVICE_NAME = "Ble Test Runner"
DEVICE_DISCOVERY_TIMEOUT = 5


async def main():
    device_found = False

    advs = await BleakScanner.discover(
        timeout=DEVICE_DISCOVERY_TIMEOUT, return_adv=True
    )

    for address, (_, data) in advs.items():
        if DEVICE_NAME == data.local_name:
            device_found = True

        print(f"{data.local_name} | {address} | {data.rssi} dBm")

    if device_found:
        print(f"Found {DEVICE_NAME} ({data.rssi} dBm)")
    else:
        print(
            f"Failed to find {DEVICE_NAME} device within {DEVICE_DISCOVERY_TIMEOUT} seconds.",
            file=sys.stderr,
        )
        sys.exit(1)


if __name__ == "__main__":
    asyncio.run(main())
