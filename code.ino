#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

/*
  Modularized Firebase LED controller (same behaviour as your original sketch)

  High-level flow:
   1) setup() initializes Serial, LEDs and WiFi
   2) loop() repeatedly:
        - requests /leds.json from your Firebase Realtime Database
        - inspects the response for "red":true, "green":true, "blue":true
        - turns the corresponding LEDs ON or OFF
        - waits 2 seconds and repeats

  Note: This sketch keeps your original simple JSON-checking approach (string search).
        For more robust JSON parsing consider using the ArduinoJson library.
*/

// ----------------------------
// === Configuration ========
// ----------------------------

// CHANGE THESE TO YOUR WIFI SSID/PASSWORD
const char* ssid     = "iPhone";
const char* password = "sam12345";

// Your Firebase Realtime DB host (without https://, no trailing slash)
const char* firebase_host = "trafficlightcontroller-8b7f7-default-rtdb.firebaseio.com";
// HTTPS port for Firebase
const int firebase_port = 443;

// The path we will request from Firebase
const char* firebase_path = "/leds.json";

// LED pin assignments (change if needed)
const int RED   = 2;
const int GREEN = 3;
const int BLUE  = 4;

// ----------------------------
// === Networking objects =====
// ----------------------------

// WiFiSSLClient provides TLS capability for HTTPS (port 443)
WiFiSSLClient wifi;

// HttpClient will use the wifi (SSL) client, the host and port specified above
HttpClient client = HttpClient(wifi, firebase_host, firebase_port);


// ----------------------------
// === Function declarations ===
// ----------------------------
// (Arduino allows functions after use, so explicit prototypes are optional.
//  I list functions here for readability.)
void initSerialAndLEDs();
void connectWiFi();
int fetchFirebase(String &outResponse);
void applyLEDsFromResponse(const String &response);
void setLED(int pin, bool on, const char *name);


// ----------------------------
// === setup() ===============
// ----------------------------
void setup() {
  // Initialize Serial, pins and WiFi using helper functions
  initSerialAndLEDs();   // sets up Serial and configures LED pins
  connectWiFi();         // attempts to connect to the WiFi network
}

// ----------------------------
// === loop() ================
// ----------------------------
void loop() {
  // Main repeating task: check Firebase and update LEDs
  Serial.println("Checking Firebase...");

  // Fetch JSON from Firebase. fetchFirebase returns the HTTP status code
  String response;
  int statusCode = fetchFirebase(response);

  // If the request succeeded (HTTP 200) parse the response for LED states
  if (statusCode == 200) {
    Serial.println("Firebase response: " + response);
    applyLEDsFromResponse(response); // inspect string for "red":true etc.
  } else {
    // Print the HTTP error code for debugging
    Serial.println("Firebase error: " + String(statusCode));
  }

  // Wait 2 seconds before polling Firebase again (same behaviour as your original)
  delay(2000);
}


// ----------------------------
// === Helper functions =======
// ----------------------------

/*
  initSerialAndLEDs()
  - Purpose: Set up Serial (for debug) and configure LED pins as outputs.
  - Steps:
      1) Start Serial at 9600 baud so we can print diagnostic messages.
      2) Configure each LED pin as OUTPUT using pinMode().
      3) Turn all LEDs OFF initially by writing LOW.
*/
void initSerialAndLEDs() {
  Serial.begin(9600);               // open serial for debugging
  while (!Serial) { /* wait for Serial (optional on some boards) */ }

  // Configure LED pins as outputs
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // Ensure LEDs start in known OFF state
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);

  Serial.println("LED pins initialized and turned OFF.");
}

/*
  connectWiFi()
  - Purpose: Connect the Arduino board to the configured WiFi network.
  - Steps:
      1) Call WiFi.begin(ssid, password) to start the connection process.
      2) Poll WiFi.status() until WL_CONNECTED is reached.
      3) Print progress to Serial (dots) and final IP address when connected.

  NOTE: This function blocks until connected (same behavior as original code).
*/
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  // Loop until connected. The original sketch blocked here; we keep that behavior.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

/*
  fetchFirebase(outResponse)
  - Purpose: Send an HTTP GET request to firebase_path and return the HTTP status code.
  - Parameters:
      outResponse (String&) - will be filled with the response body (if any).
  - Returns:
      HTTP status code returned by the HttpClient (e.g., 200 on success).
  - Steps:
      1) Use client.get(path) to issue the GET request.
      2) Read the status code via client.responseStatusCode().
      3) Read the body via client.responseBody() and store in outResponse.
*/
int fetchFirebase(String &outResponse) {
  // Issue GET request to the path we set earlier
  client.get(firebase_path);

  // Get HTTP response code (200 = OK)
  int status = client.responseStatusCode();

  // Read the response body as a String
  outResponse = client.responseBody();

  // Return status so caller can decide what to do
  return status;
}

/*
  applyLEDsFromResponse(response)
  - Purpose: Inspect the Firebase JSON response (as a string) and set LEDs accordingly.
  - Steps:
      1) Check if response contains the substring "\"red\":true". If yes, turn RED on and print.
         Otherwise, turn RED off.
      2) Repeat the same for GREEN and BLUE.
  - Note: This uses simple substring checks exactly like the original sketch, so it will
          match responses like: {"red":true,"green":false,"blue":true}
*/
void applyLEDsFromResponse(const String &response) {
  // Check for the exact pattern "red":true in the returned JSON (string search)
  if (response.indexOf("\"red\":true") >= 0) {
    setLED(RED, true, "Red");
  } else {
    setLED(RED, false, "Red");
  }

  // Check for green
  if (response.indexOf("\"green\":true") >= 0) {
    setLED(GREEN, true, "Green");
  } else {
    setLED(GREEN, false, "Green");
  }

  // Check for blue
  if (response.indexOf("\"blue\":true") >= 0) {
    setLED(BLUE, true, "Blue");
  } else {
    setLED(BLUE, false, "Blue");
  }
}

/*
  setLED(pin, on, name)
  - Purpose: Central helper that sets the digital pin HIGH/LOW and prints a message when turned ON.
  - Parameters:
      pin  - Arduino pin number connected to the LED
      on   - boolean: true => turn LED ON, false => turn LED OFF
      name - human-friendly name of the LED used for Serial prints
  - Behavior:
      - If 'on' is true, digitalWrite(pin, HIGH) and print "<Name> LED ON"
      - If 'on' is false, digitalWrite(pin, LOW) and do not print (preserves original behaviour)
*/
void setLED(int pin, bool on, const char *name) {
  if (on) {
    digitalWrite(pin, HIGH);
    Serial.print(name);
    Serial.println(" LED ON");
  } else {
    digitalWrite(pin, LOW);
    // Intentionally do not print when turning OFF to match original behaviour.
  }
}
