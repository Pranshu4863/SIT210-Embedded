import RPi.GPIO as GPIO  
import time               

# Define the pin numbering mode. BOARD means we use the physical pin numbers on the Pi.
GPIO.setmode(GPIO.BOARD)

# pin being used in pi
led_pin = 10

# Setup the LED pin as an output pin.
GPIO.setup(led_pin, GPIO.OUT)

try:
    print("LED blinking started. Press Ctrl+C to stop.")
    while True:
        GPIO.output(led_pin, GPIO.HIGH)  # Turn the LED on
        time.sleep(0.5)                  # Wait for 0.5 seconds
        GPIO.output(led_pin, GPIO.LOW)   # Turn the LED off
        time.sleep(0.5)                  # Wait for 0.5 seconds

except KeyboardInterrupt:
    print("\nProgram stopped by user.")
    
finally:
    print("Cleaning up GPIO settings.")
    GPIO.cleanup()  # Resets all GPIO ports we've used back to safe input mode
