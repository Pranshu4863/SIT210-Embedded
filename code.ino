# blinker.py
# This program blinks an LED connected to GPIO pin 10.

# Import the necessary libraries
import RPi.GPIO as GPIO  # Library to control the GPIO pins
import time               # Library to add delays

# Define the pin numbering mode. BOARD means we use the physical pin numbers on the Pi.
GPIO.setmode(GPIO.BOARD)

# Define the pin number we are using for the LED output.
led_pin = 10

# Setup the LED pin as an output pin.
GPIO.setup(led_pin, GPIO.OUT)

# The main program logic is inside a try-except block.
# This allows us to clean up the GPIO pins if the user stops the program with Ctrl+C.
try:
    print("LED blinking started. Press Ctrl+C to stop.")
    # This while loop runs forever until the program is interrupted.
    while True:
        GPIO.output(led_pin, GPIO.HIGH)  # Turn the LED on
        time.sleep(0.5)                  # Wait for 0.5 seconds
        GPIO.output(led_pin, GPIO.LOW)   # Turn the LED off
        time.sleep(0.5)                  # Wait for 0.5 seconds

except KeyboardInterrupt:
    # This block runs only if Ctrl+C is pressed.
    print("\nProgram stopped by user.")
    
finally:
    # The 'finally' block always runs, ensuring a clean exit.
    print("Cleaning up GPIO settings.")
    GPIO.cleanup()  # Resets all GPIO ports we've used back to safe input mode
