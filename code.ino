#!/usr/bin/env python3
# Shebang line: tells the operating system to run this file with the system's python3 interpreter.
"""
led_gui_tkinter.py
SIT210 5.1P - Tkinter GUI to control 3 LEDs (Red, Green, Blue)
Works on Raspberry Pi with gpiozero. Includes a simulation fallback so you can run
it on a PC for testing (it will print simulated LED activity).
"""
# Module docstring above: provides a brief description of the script's purpose.

import sys
# Import the standard 'sys' module. It's included in case we need system-related functionality
# (for example to exit the program or to access command-line arguments). Not used directly
# in the rest of the script but commonly kept for future extension or debugging.

import tkinter as tk
# Import the tkinter GUI package and alias it as 'tk' so we can reference tkinter classes
# and functions with the shorter prefix 'tk'.

from tkinter import ttk
# Import the tkinter 'ttk' themed widget set which provides nicer-looking widgets
# (like Buttons, Frames and Radiobuttons) compared to the default tkinter widgets.

# Try to import gpiozero; if not available we simulate (so code can run on non-Pi)
# This comment is the original explanatory comment and has been left unchanged.

SIMULATE = False
# Initialize a flag 'SIMULATE' to False. We'll set this to True if gpiozero cannot be imported,
# meaning the code should run in simulation mode (useful when running on a PC rather than a Pi).

try:
    from gpiozero import LED
    # Attempt to import the 'LED' class from the gpiozero library which controls GPIO pins
    # on a Raspberry Pi. If this succeeds, we will use the real gpiozero LED objects.
except Exception:
    # If any error occurs while importing (e.g. gpiozero not installed or not running on a Pi),
    # fall back to a simple simulated LED class so the rest of the script can still run.
    SIMULATE = True
    class LED:
        # A minimal replacement for gpiozero.LED used only for simulation/testing on non-Pi systems.
        def _init_(self, pin):
            self.pin = pin
            # Store the pin number so the simulated LED can print which pin it is emulating.
            self._is_on = False
            # Track an internal boolean state indicating whether the simulated LED is on.
        def on(self):
            # Simulate turning the LED on by setting the internal state and printing a message.
            self._is_on = True
            print(f"[SIM] LED pin {self.pin} ON")
        def off(self):
            # Simulate turning the LED off by clearing the internal state and printing a message.
            self._is_on = False
            print(f"[SIM] LED pin {self.pin} OFF")

# BCM pin assignment (change if you want different pins)
# This comment is original and kept as-is. The constants below assign GPIO pins using
# the Broadcom (BCM) numbering scheme which gpiozero uses by default.
RED_PIN = 17
# RED_PIN: GPIO pin number for the red LED (BCM numbering).
GREEN_PIN = 27
# GREEN_PIN: GPIO pin number for the green LED (BCM numbering).
BLUE_PIN = 22
# BLUE_PIN: GPIO pin number for the blue LED (BCM numbering).

led_red = LED(RED_PIN)
# Create an LED object for the red LED using the chosen pin. If gpiozero was imported
# this is a real LED object controlling the hardware; otherwise it's the simulated class.
led_green = LED(GREEN_PIN)
# Create an LED object for the green LED.
led_blue = LED(BLUE_PIN)
# Create an LED object for the blue LED.

def set_led(color):
    """Turn one LED on and others off. color is 'red', 'green', 'blue', or 'off'."""
    # Function docstring: explains the behavior and the accepted values for the 'color' parameter.
    if color == 'red':
        # If the requested color is 'red', turn the red LED on and ensure the others are off.
        led_red.on();   led_green.off(); led_blue.off()
    elif color == 'green':
        # If the requested color is 'green', turn the green LED on and ensure the others are off.
        led_red.off();  led_green.on();  led_blue.off()
    elif color == 'blue':
        # If the requested color is 'blue', turn the blue LED on and ensure the others are off.
        led_red.off();  led_green.off(); led_blue.on()
    else:  # 'off' or unknown
        # For 'off' or any unknown value, turn all LEDs off to be safe.
        led_red.off();  led_green.off(); led_blue.off()

# --- GUI setup ---
# The following lines create the main Tkinter window and configure basic layout and widgets.
root = tk.Tk()
# Create the main application window (an instance of Tk).
root.title("RPi LED Controller - SIT210 5.1P")
# Set the window title shown in the title bar.
root.resizable(False, False)
# Disable window resizing in both width and height so the layout stays fixed.

frame = ttk.Frame(root, padding=12)
# Create a ttk Frame widget to hold other widgets, with 12 pixels of internal padding.
frame.grid()
# Use the grid geometry manager to place the frame into the main window. This call places
# the frame in the default (0,0) cell and allows us to use grid for child widgets.

selected = tk.StringVar(value='off')  # default: all off
# Create a tkinter StringVar to hold the currently selected radio button value.
# Initialize it to 'off' so that all LEDs are off by default.

def on_radio_change():
    # Callback function invoked whenever a radio button selection changes. It reads the
    # current value from the 'selected' StringVar and calls set_led to update the hardware.
    set_led(selected.get())

# Radiobuttons for the three LEDs + an "All Off" option
# Create four themed radiobutton widgets tied to the same StringVar so only one can be
# selected at a time. Each one specifies a value and the callback command to run when clicked.
rb_red = ttk.Radiobutton(frame, text='Red',   value='red',   variable=selected, command=on_radio_change)
rb_green = ttk.Radiobutton(frame, text='Green', value='green', variable=selected, command=on_radio_change)
rb_blue = ttk.Radiobutton(frame, text='Blue',  value='blue',  variable=selected, command=on_radio_change)
rb_off = ttk.Radiobutton(frame, text='All Off', value='off', variable=selected, command=on_radio_change)

rb_red.grid(column=0, row=0, sticky='w', pady=2)
# Place the red radiobutton at column 0, row 0. 'sticky="w"' aligns it to the left (west).
# 'pady=2' adds 2 pixels of vertical spacing around the widget.
rb_green.grid(column=0, row=1, sticky='w', pady=2)
# Place the green radiobutton beneath the red one.
rb_blue.grid(column=0, row=2, sticky='w', pady=2)
# Place the blue radiobutton beneath the green one.
rb_off.grid(column=0, row=3, sticky='w', pady=2)
# Place the 'All Off' radiobutton beneath the blue one.

# Exit button  turn all LEDs off then quit
# The original comment above is preserved. Below we define a helper that ensures the LEDs
# are turned off before the GUI exits so the hardware is left in a safe state.
def clean_exit():
    # When exiting, make sure all LEDs are off to avoid leaving hardware in an unexpected state.
    set_led('off')
    # Destroy the main window and exit the Tk main loop.
    root.destroy()

exit_btn = ttk.Button(frame, text='Exit', command=clean_exit)
# Create a button labeled 'Exit' which calls clean_exit when clicked.
exit_btn.grid(column=0, row=4, pady=(10,0))
# Place the Exit button below the radio buttons, adding 10 pixels of extra top padding.

root.protocol("WM_DELETE_WINDOW", clean_exit)
# Register the clean_exit function as the handler for the window manager close event
# (clicking the window's close 'X' button). This ensures the same cleanup runs whether
# the user clicks Exit or closes the window.

if _name_ == '_main_':
    # Standard Python idiom: only start the GUI mainloop if the script is executed directly
    # (and not imported as a module by another script).
    root.mainloop()
    # Enter the Tk event loop which waits for user interaction and keeps the window open.
