// Arduino Nano 33 IoT: BH1750 -> BLE notify
#include <Wire.h> 
#include <BH1750.h>
#include <ArduinoBLE.h>

BH1750 lightMeter;

// BLE service and characteristic UUIDs
BLEService lightService("12345678-1234-5678-1234-56789abcdef0"); 
BLECharacteristic luxChar("12345678-1234-5678-1234-56789abcdef1", BLERead | BLENotify, 20); // up to 20 bytes

void setup() { 
  Serial.begin(115200);
  while (!Serial);

  // Initialize BH1750
  Wire.begin();
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) { // Start with high resolution mode
    Serial.println("BH1750 not found. Check wiring.");
    while (1);
  }
  Serial.println("BH1750 ready");

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }

  BLE.setLocalName("BH1750Sensor"); // device name
  BLE.setAdvertisedService(lightService); // add the service UUID

  lightService.addCharacteristic(luxChar); // add the characteristic to the service 
  BLE.addService(lightService);

  luxChar.writeValue("0"); // initial value
  BLE.advertise();
  Serial.println("BLE advertising as BH1750Sensor");
}

void loop() {
  BLEDevice central = BLE.central(); // Check for central connection

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      float lux = lightMeter.readLightLevel();
      if (isnan(lux)) {
        Serial.println("BH1750 read error");
      } else {
        unsigned long lux_int = (unsigned long)(lux + 0.5);
        char buf[16];
        snprintf(buf, sizeof(buf), "%lu", lux_int);

        // Only notify if a client is connected
        if (BLE.connected()) {
          luxChar.writeValue((const unsigned char*)buf, strlen(buf));
        }

        Serial.print("Lux: ");
        Serial.println(buf);
      }
      delay(500); // 0.5 second interval
      BLE.poll(); // keep BLE stack running smoothly
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }

  // Keep advertising if no central is connected
  BLE.poll();
}
