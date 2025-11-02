#!/usr/bin/env python3
"""
led_pwm_gui.py
SIT210 - Task 5.2C
Raspberry Pi Tkinter GUI with 3 sliders to control LED brightness using PWM
"""

# Import the tkinter library for GUI development
import tkinter as tk
from tkinter import ttk  # ttk gives access to themed widgets (modern-looking UI elements)

# gpiozero PWMLED for brightness control
try:
    # Try importing PWMLED from gpiozero (works only on Raspberry Pi with GPIO access)
    from gpiozero import PWMLED
    SIMULATE = False  # If import works, we are not simulating
except ImportError:
    # If gpiozero is not available (e.g., running on non-Raspberry Pi), enable simulation
    SIMULATE = True
    # Define a mock PWMLED class to simulate LED behavior in console
    class PWMLED:
        def __init__(self, pin): 
            # Save pin number and initialize brightness to 0
            self.pin, self.value = pin, 0
        def __setattr__(self, k, v): 
            # Whenever the brightness 'value' is updated, print simulated output
            if k=="value": 
                print(f"[SIM] LED on pin {self.pin} brightness {v:.2f}")
            # Call the parent class __setattr__ to actually set the attribute
            super().__setattr__(k,v)

# Pin assignments (using BCM GPIO numbering for Raspberry Pi)
RED_PIN, GREEN_PIN, BLUE_PIN = 17, 27, 22

# Create PWMLED objects for each LED color (connected to corresponding GPIO pins)
led_red   = PWMLED(RED_PIN)
led_green = PWMLED(GREEN_PIN)
led_blue  = PWMLED(BLUE_PIN)

# --- GUI setup ---
root = tk.Tk()  # Create the main Tkinter application window
root.title("LED Brightness Control - SIT210 5.2C")  # Set window title
root.resizable(False, False)  # Disable window resizing (fixed size window)

# Create a frame (container) inside the main window with padding around widgets
frame = ttk.Frame(root, padding=15)
frame.grid()  # Use grid layout manager to arrange widgets

# --- Slider callback functions (called when user moves sliders) ---
def update_red(val):   
    # Convert slider value (0–100) to 0–1.0 and update red LED brightness
    led_red.value   = float(val)/100

def update_green(val): 
    # Convert slider value to brightness for green LED
    led_green.value = float(val)/100

def update_blue(val):  
    # Convert slider value to brightness for blue LED
    led_blue.value  = float(val)/100

# --- Add sliders to control LED brightness ---
# Red LED slider
ttk.Label(frame, text="Red LED").grid(row=0, column=0, sticky="w")  # Label for red slider
slider_red = tk.Scale(frame, from_=0, to=100, orient="horizontal", command=update_red)  
# Slider ranges from 0 to 100, horizontal orientation, calls update_red() when moved
slider_red.grid(row=0, column=1, padx=10, pady=5)  # Place slider next to label

# Green LED slider
ttk.Label(frame, text="Green LED").grid(row=1, column=0, sticky="w")  # Label for green slider
slider_green = tk.Scale(frame, from_=0, to=100, orient="horizontal", command=update_green)
slider_green.grid(row=1, column=1, padx=10, pady=5)

# Blue LED slider
ttk.Label(frame, text="Blue LED").grid(row=2, column=0, sticky="w")  # Label for blue slider
slider_blue = tk.Scale(frame, from_=0, to=100, orient="horizontal", command=update_blue)
slider_blue.grid(row=2, column=1, padx=10, pady=5)

# --- Exit button and cleanup ---
def clean_exit():
    # Before closing, turn off all LEDs by setting brightness to 0
    led_red.value = led_green.value = led_blue.value = 0
    # Destroy (close) the Tkinter window
    root.destroy()

# Add Exit button to GUI, linked to clean_exit() function
exit_btn = ttk.Button(frame, text="Exit", command=clean_exit)
exit_btn.grid(row=3, column=0, columnspan=2, pady=10)

# Handle window close button (X) the same way as Exit button
root.protocol("WM_DELETE_WINDOW", clean_exit)

# --- Main program loop ---
if __name__ == "__main__":
    # Start the Tkinter event loop (keeps the GUI running until closed)
    root.mainloop()
