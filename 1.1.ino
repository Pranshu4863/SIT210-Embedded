const int ledPin = 13;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void dot() {
  digitalWrite(ledPin, HIGH);
  delay(250);
  digitalWrite(ledPin, LOW);
  delay(250);
}

void dash() {
  digitalWrite(ledPin, HIGH);
  delay(750);
  digitalWrite(ledPin, LOW);
  delay(250);
}

// Morse functions for each letter
void morseP() { dot(); dash(); dash(); dot(); delay(750); }
void morseR() { dot(); dash(); dot(); delay(750); }
void morseA() { dot(); dash(); delay(750); }
void morseN() { dash(); dot(); delay(750); }
void morseS() { dot(); dot(); dot(); delay(750); }
void morseH() { dot(); dot(); dot(); dot(); delay(750); }
void morseU() { dot(); dot(); dash(); delay(750); }

void loop() {
  morseP();
  morseR();
  morseA();
  morseN();
  morseS();
  morseH();
  morseU();
  delay(2000); // Pause before repeating
}
