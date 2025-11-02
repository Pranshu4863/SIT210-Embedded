#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Servo.h>

#define LDR_TOP A0
#define LDR_BOTTOM A1
#define SERVO_PIN 9
#define RELAY_PIN 8

Servo servoMotor;
Adafruit_INA219 ina219;

float voltage=0, current_mA=0, power_mW=0;
int servoPos=90;

// BLE Service + Characteristics
BLEService solarService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEFloatCharacteristic voltageChar("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic currentChar("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic powerChar("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEByteCharacteristic relayChar("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify);

unsigned long lastUpdate=0;

void setup() {
  Serial.begin(115200);
  while(!Serial); // Wait for Serial to attach

  Serial.println("Starting Smart Grid Node...");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(servoPos);
  Wire.begin();

  if(!ina219.begin()) {
    Serial.println("INA219 not detected!");
  } else {
    Serial.println("INA219 ready.");
  }

  if(!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    while(1);
  }

  BLE.setLocalName("SmartGridNode");
  BLE.setAdvertisedService(solarService);
  solarService.addCharacteristic(voltageChar);
  solarService.addCharacteristic(currentChar);
  solarService.addCharacteristic(powerChar);
  solarService.addCharacteristic(relayChar);
  BLE.addService(solarService);
  BLE.advertise();

  Serial.println("BLE Smart Grid Node ready. Advertising as 'SmartGridNode'.");
}


void loop(){
  BLEDevice central = BLE.central();
  if(central){
    Serial.print("Connected: "); Serial.println(central.address());
    while(central.connected()){
      if(millis()-lastUpdate>1000){
        readSensors();
        sunTrack();
        updateBLE();
        lastUpdate=millis();
      }
      if(relayChar.written()){
        if(relayChar.value()) digitalWrite(RELAY_PIN,HIGH);
        else digitalWrite(RELAY_PIN,LOW);
      }
    }
    Serial.println("Disconnected");
  }
}

void readSensors(){
  voltage=ina219.getBusVoltage_V();
  current_mA=ina219.getCurrent_mA();
  power_mW=ina219.getPower_mW();
}

void updateBLE() {
  voltageChar.writeValue(voltage);
  currentChar.writeValue(current_mA);
  powerChar.writeValue(power_mW);
  relayChar.writeValue(digitalRead(RELAY_PIN));

  Serial.print("V:");
  Serial.print(voltage, 2);
  Serial.print("  I:");
  Serial.print(current_mA, 1);
  Serial.print("  P:");
  Serial.println(power_mW, 1);
}


void sunTrack(){
  int t=analogRead(LDR_TOP), b=analogRead(LDR_BOTTOM);
  int diff=t-b;
  if(abs(diff)>20){
    if(diff>0) servoPos=constrain(servoPos-1,0,180);
    else servoPos=constrain(servoPos+1,0,180);
    servoMotor.write(servoPos);
  }
}
