/* 
   SIT210 - Task 4.2C (Arduino Nano 33 IoT)
   Multiple Interrupts Example

   - Push button on pin D2 will toggle LED1
   - Interrupts detect button presses and sensor triggers
   - Main loop checks "flags" and updates LEDs with debounce
*/

const uint8_t BUTTON_PIN = 2;   // Pushbutton connected here
const uint8_t SENSOR_PIN = 3;   // PIR sensor output connected here
const uint8_t LED1_PIN   = 5;   // LED1 connected here
const uint8_t LED2_PIN   = 6;   // LED2 connected here

// These "flags" are signals from the interrupt routines
// They are set to 'true' when an event happens
volatile bool buttonFlag = false;
volatile bool sensorFlag = false;

// These variables are used for debouncing (avoiding false multiple triggers)
unsigned long lastButtonMillis = 0;
unsigned long lastSensorMillis = 0;
const unsigned long DEBOUNCE_MS = 200; // 200ms = 0.2 second debounce window

// Variables to remember the ON/OFF state of LEDs
bool led1State = false;
bool led2State = false;

/* 
   setup() runs once when the board is powered or reset.
   Here we set up Serial communication, configure pins,
   and attach the interrupt functions.
*/
void setup() {
  Serial.begin(115200);              // Start Serial Monitor at 115200 baud
  while (!Serial) { }                // Wait until Serial Monitor opens

  // Configure LED pins as outputs
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);       // Start with LEDs OFF
  digitalWrite(LED2_PIN, LOW);

  // Configure button pin with internal pull-up resistor
  // This means the button pin will be normally HIGH, and when pressed it goes LOW
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Configure sensor pin as input
  pinMode(SENSOR_PIN, INPUT);

  // Attach interrupts:
  // When button goes from HIGH to LOW (pressed), call buttonISR()
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  // When sensor goes from LOW to HIGH (motion detected), call sensorISR()
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), sensorISR, RISING);

  Serial.println("Task4.2C initialized. Waiting for button or sensor events...");
}

/* 
   loop() runs again and again forever.
   We check the flags set by the interrupt routines,
   handle debouncing, and update LEDs + print messages.
*/
void loop() {
  // --- Handle Button Events ---
  if (buttonFlag) {                        // If button ISR has set the flag
    unsigned long now = millis();          // Current time in ms
    if (now - lastButtonMillis >= DEBOUNCE_MS) {  // Check debounce
      lastButtonMillis = now;              // Update last valid press time
      led1State = !led1State;              // Toggle LED1 state (ON -> OFF -> ON ...)
      digitalWrite(LED1_PIN, led1State ? HIGH : LOW);

      Serial.print("Button interrupt -> LED1 is now: ");
      Serial.println(led1State ? "ON" : "OFF");
    }
    buttonFlag = false;                    // Clear the flag after handling
  }

  // --- Handle Sensor Events ---
  if (sensorFlag) {                        // If sensor ISR has set the flag
    unsigned long now = millis();
    if (now - lastSensorMillis >= DEBOUNCE_MS) {  // Check debounce
      lastSensorMillis = now;
      led2State = !led2State;              // Toggle LED2 state
      digitalWrite(LED2_PIN, led2State ? HIGH : LOW);

      Serial.print("Sensor interrupt -> LED2 is now: ");
      Serial.println(led2State ? "ON" : "OFF");
    }
    sensorFlag = false;                    // Clear the flag after handling
  }

  delay(10);  // Small delay to avoid busy looping
}

/* 
   buttonISR() is the Interrupt Service Routine for the button.
   It is called automatically when the button is pressed.
   We DO NOT do heavy work here — just set a flag.
*/
void buttonISR() {
  buttonFlag = true;  // Tell the main loop that the button was pressed
}

/* 
   sensorISR() is the Interrupt Service Routine for the PIR sensor.
   It is called automatically when the sensor detects motion.
   Again, we only set a flag here.
*/
void sensorISR() {
  sensorFlag = true;  // Tell the main loop that the sensor triggered
}
