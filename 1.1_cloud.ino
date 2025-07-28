#include "thingProperties.h"

#define LED_PIN LED_BUILTIN

void setup() {
  Serial.begin(9600);
  delay(1500);

  pinMode(LED_PIN, OUTPUT);
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();

  if (blinkControl) {
    blinkNameInMorse();
    delay(2000);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

// Morse code for "PRANSHU"
void blinkNameInMorse() {
  blinkLetter('P'); delay(1000);
  blinkLetter('R'); delay(1000);
  blinkLetter('A'); delay(1000);
  blinkLetter('N'); delay(1000);
  blinkLetter('S'); delay(1000);
  blinkLetter('H'); delay(1000);
  blinkLetter('U'); delay(2000); // End of name
}

void blinkDot() {
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
  delay(200);
}

void blinkDash() {
  digitalWrite(LED_PIN, HIGH);
  delay(600);
  digitalWrite(LED_PIN, LOW);
  delay(200);
}

void blinkLetter(char c) {
  switch (toupper(c)) {
    case 'A': blinkDot(); blinkDash(); break;           // .-
    case 'H': blinkDot(); blinkDot(); blinkDot(); blinkDot(); break; // ....
    case 'N': blinkDash(); blinkDot(); break;           // -.
    case 'P': blinkDot(); blinkDash(); blinkDash(); blinkDot(); break; // .--.
    case 'R': blinkDot(); blinkDash(); blinkDot(); break; // .-.
    case 'S': blinkDot(); blinkDot(); blinkDot(); break; // ...
    case 'U': blinkDot(); blinkDot(); blinkDash(); break; // ..-
    default: break;
  }
}

void onBlinkControlChange() {
  Serial.print("blinkControl changed to: ");
  Serial.println(blinkControl ? "true" : "false");
}
