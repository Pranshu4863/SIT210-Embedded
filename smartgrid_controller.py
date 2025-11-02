#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import asyncio, struct, time, random
from bleak import BleakScanner, BleakClient

# === BLE UUIDs (must match Arduino code) ===
UUID_VOLT = "19B10001-E8F2-537E-4F6C-D104768A1214"
UUID_CURR = "19B10002-E8F2-537E-4F6C-D104768A1214"
UUID_PWR  = "19B10003-E8F2-537E-4F6C-D104768A1214"
UUID_RELAY= "19B10004-E8F2-537E-4F6C-D104768A1214"

# === Simulated Node 2 Function ===
def get_node2_voltage():
    """Simulate voltage for virtual Node 2 (± small variation)."""
    return 12.5 + random.uniform(-0.3, 0.3)

# === BLE Helper ===
async def read_float(client, uuid):
    """Reads a 4-byte float characteristic from BLE and converts it."""
    raw = await client.read_gatt_char(uuid)
    return struct.unpack('<f', raw)[0]

async def connect_to_device():
    """Scan for SmartGridNode and connect automatically."""
    print("Scanning for SmartGridNode...")
    device = None
    devices = await BleakScanner.discover(timeout=10.0)
    for d in devices:
        if d.name and "SmartGridNode" in d.name:
            device = d
            break

    if not device:
        print("SmartGridNode not found. Make sure Arduino is powered and advertising.")
        return None

    print(f"Found SmartGridNode at {device.address}")
    client = BleakClient(device.address)
    await client.connect()
    print("Connected successfully!")
    return client

# === Main Smart Grid Logic ===
async def main():
    client = await connect_to_device()
    if not client:
        return

    active_node = "node1"
    try:
        while True:
            # --- Read Node 1 data ---
            try:
                v1 = await read_float(client, UUID_VOLT)
                i1 = await read_float(client, UUID_CURR)
                p1 = await read_float(client, UUID_PWR)
            except Exception as e:
                print("Read error:", e)
                break

            # --- Simulate Node 2 data ---
            v2 = get_node2_voltage()
            p2 = v2 * (random.uniform(0.4, 0.6))  # simulate power roughly

            # --- Print current readings ---
            print(f"Node1: {v1:.2f} V, {i1:.1f} mA, {p1:.1f} mW | Node2: {v2:.2f} V | Active: {active_node}", end=" ")

            # --- Decision Logic ---
            if v2 > v1 + 0.2 and active_node != "node2":
                await client.write_gatt_char(UUID_RELAY, bytearray([0]))  # Turn off Node1 relay
                active_node = "node2"
                print("-> Switched to Node2 (Simulated)")
            elif v1 >= v2 and active_node != "node1":
                await client.write_gatt_char(UUID_RELAY, bytearray([1]))  # Turn on Node1 relay
                active_node = "node1"
                print("-> Switched to Node1 (Real)")
            else:
                print()

            await asyncio.sleep(3)

    except KeyboardInterrupt:
        print("\nStopping...")
    finally:
        if client and client.is_connected:
            await client.disconnect()
            print("Disconnected from SmartGridNode.")

# === Run ===
if __name__ == "__main__":
    asyncio.run(main())
