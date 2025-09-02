#include <WiFiNINA.h>
#include <PubSubClient.h>

// ===== WiFi credentials =====
const char* ssid = "iPhone";       
const char* password = "sam12345";      

// ===== MQTT broker =====
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;   // use standard MQTT port
const char* mqttTopicWave  = "SIT210/wave";
const char* mqttTopicPat   = "SIT210/pat";

// ===== Objects =====
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// ===== Pins =====
const int trigPin = 2;   // Ultrasonic TRIG
const int echoPin = 3;   // Ultrasonic ECHO
const int ledPin  = 7;   // LED

// ===== Timing control =====
unsigned long lastWave = 0;
unsigned long lastPat = 0;
const unsigned long cooldown = 2000;  // 2 sec cooldown to prevent spamming

// ===== Function to get distance =====
long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30 ms
  return duration * 0.034 / 2; // distance in cm
}

// ===== LED Flashing =====
void flashLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(delayMs);
    digitalWrite(ledPin, LOW);
    delay(delayMs);
  }
}

// ===== MQTT Callback =====
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (String(topic) == mqttTopicWave) {
    flashLED(3, 300);   // Blink LED 3 times for wave
  }
  else if (String(topic) == mqttTopicPat) {
    flashLED(5, 150);   // Blink LED 5 times fast for pat
  }
}

// ===== Reconnect to MQTT if disconnected =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print(mqttServer);
    Serial.print(":");
    Serial.println(mqttPort);

    // Generate a unique client ID each time
    String clientId = "pranshu_webclient_01";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("✅ MQTT connected!");
      client.subscribe(mqttTopicWave);
      client.subscribe(mqttTopicPat);
    } else {
      Serial.print("❌ failed, rc=");
      Serial.print(client.state());
      Serial.println(" (retry in 2s)");
      delay(2000);
    }
  }
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("✅ Connected to WiFi! IP: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Seed random generator (for clientId)
  randomSeed(analogRead(0));
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // Detect "wave" (hand < 15 cm)
  if (distance > 0 && distance <= 15 && millis() - lastWave > cooldown) {
    client.publish(mqttTopicWave, "Pranshu");
    Serial.println("➡ Published: Wave detected by Pranshu!");
    lastWave = millis();
  }

  delay(200); // small delay to avoid flooding Serial
}
