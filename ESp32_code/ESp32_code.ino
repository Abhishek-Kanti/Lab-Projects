#include <WiFi.h>
#include <FirebaseESP32.h>

// WiFi and Firebase credentials
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "Password"
#define led 15

#define API_KEY "AIzaSyB-L2pj_4XVH9y7VBYOSAsTsT8p4cveHZY"
#define DATABASE_URL "https://foodai-7ebf0-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(9600); // Debugging
  pinMode(led, OUTPUT);
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    // Serial.print(".");
    digitalWrite(led, 1);
    delay(500);
    digitalWrite(led, 0);
    delay(500);
  }
  //Serial.println("\nWiFi Connected!");
  digitalWrite(led, 0);

  // Firebase Config
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Anonymous sign-up
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signup successful");
  } else {
    Serial.print("Firebase signup failed: ");
    Serial.println(config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  delay(2000); // small delay
}

void loop() {
  // Read command from Firebase
  if (Firebase.getString(fbdo, "/motor_control/command")) {
    String command = fbdo.stringData();
    // Serial.print("Received command from Firebase: ");
    Serial.println(command);
  } else {
    //Serial.print("Error: ");
    //Serial.println(fbdo.errorReason());
    digitalWrite(led, 1);
  }

  delay(1000); // Poll every second
}



