import asyncio # For asynchronous operations
import RPi.GPIO as GPIO # For controlling GPIO pins on Raspberry Pi
from bleak import BleakScanner, BleakClient # For BLE communication

DEVICE_NAME = "BH1750Sensor" # Name of the BLE device
CHAR_UUID = "12345678-1234-5678-1234-56789abcdef1" # UUID of the characteristic that sends lux data 
LED_PIN = 17  # GPIO17 (pin 11)
THRESHOLD = 200  # lux value below which the LED turns ON

GPIO.setmode(GPIO.BCM) 
GPIO.setup(LED_PIN, GPIO.OUT)
GPIO.output(LED_PIN, GPIO.LOW)

def handle_notification(sender, data): # Callback for handling incoming notifications
    try:
        lux = int(data.decode().strip()) # Decode and convert the received data to an integer
        print(f"? Lux = {lux}") # Print the lux value

        if lux < THRESHOLD: 
            GPIO.output(LED_PIN, GPIO.LOW) 
            print("? Dark detected ? LED OFF")
        else:
            GPIO.output(LED_PIN, GPIO.HIGH)
            print("?? Bright ? LED ON")

    except Exception as e:
        print("?? Error decoding data:", e)

async def run(): # Main async function to manage BLE connection and notifications
    print("? Scanning for device...") 
    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0) # Scan for the device by name with a timeout of 10 seconds
    if not device:
        print("? Device not found.")
        return

    print("? Found:", device)
    async with BleakClient(device) as client:
        print("? Connected. Listening for lux updates...")
        await client.start_notify(CHAR_UUID, handle_notification)

        while True:
            await asyncio.sleep(1)

if _name_ == "_main_":
    try:
        asyncio.run(run()) 
    except KeyboardInterrupt:
        print("? Cleaning up...")
    finally:
        GPIO.cleanup()
