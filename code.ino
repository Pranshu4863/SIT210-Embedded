#include "thingProperties.h"
#include <WiFiNINA.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ThingSpeak API Details
String apiKey = "JEQDLCI4DWRQ8YI2";
const char* server = "api.thingspeak.com";

WiFiClient client;

unsigned long previousMillis = 0;
const long interval = 30000;  // 30 seconds

void setup() {
  Serial.begin(9600);
  delay(1500);

  // Initialize properties
  initProperties();

  // Connect to Arduino IoT Cloud & Wi-Fi
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Initialize DHT Sensor
  dht.begin();
}

void onTemperatureChange() {
  // Not used, but must be defined to fix linker error.
}

void loop() {
  ArduinoCloud.update();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float temp = dht.readTemperature();
    if (isnan(temp)) {
      Serial.println("Failed to read from DHT sensor!");
      temperature = 0;  // Update Cloud Variable
    } else {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.println(" °C");
      temperature = temp;  // Update Cloud Variable

      // Send Data to ThingSpeak
      if (client.connect(server, 80)) {
        String url = "/update?api_key=" + apiKey + "&field1=" + String(temp);
        Serial.print("Requesting URL: ");
        Serial.println(url);

        client.print("GET " + url + " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Connection: close\r\n\r\n");
        delay(2000);  // Wait for ThingSpeak to process
        client.stop();
        Serial.println("Data sent to ThingSpeak!");
      } else {
        Serial.println("Connection to ThingSpeak failed!");
      }
    }
  }
}
