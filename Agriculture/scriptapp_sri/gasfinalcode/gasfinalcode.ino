#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "AKCSIT407";
const char* password = "Akcsit#407";
const char* mqtt_server = "192.15.2.105";  // Replace with your MQTT broker's IP address

WiFiClient espClient;
PubSubClient client(espClient);

const int gasSensorPin = A0;  // Analog pin for MG-811 sensor

long lastMeasure = 0;
const char* nodeID = "node4";  // Unique identifier for this NodeMCU

// Calibration variables (replace with your calculated values)
const float voltageToCO2Factor = -0.12;  // Calculated slope (m)
const float zeroPointCO2 = 700;       // Calculated intercept (b)

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(nodeID)) {  // Unique client ID for NodeMCU
      Serial.println("connected");
      client.subscribe("/esp8266/gas");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.print("Calibration slope (m): ");
  Serial.println(voltageToCO2Factor);
  Serial.print("Calibration intercept (b): ");
  Serial.println(zeroPointCO2);
}

float calculateCO2Concentration(float sensorVoltage) {
  float co2Concentration = (sensorVoltage * voltageToCO2Factor) + zeroPointCO2;
  return co2Concentration;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
  
  long now = millis();
  if (now - lastMeasure > 10000) {
    lastMeasure = now;
    
    int rawGasValue = analogRead(gasSensorPin);  // Raw analog reading
    float sensorVoltage = rawGasValue * (3300 / 1024.0);  // Convert ADC value to voltage (assuming 3.3V reference)
    float co2Concentration = calculateCO2Concentration(sensorVoltage);
    
    static char payload[50];
    snprintf(payload, 50, "%.2f", co2Concentration);

    // Publish to a unique topic for this node
    String topic = String("/esp8266/gas/") + nodeID;
    client.publish(topic.c_str(), payload, true);  // Publish with QoS 1 to ensure message delivery
    
    // Print values to Serial Monitor
    Serial.print("Raw Gas Sensor Value: ");
    Serial.println(rawGasValue);
    Serial.print("Sensor Voltage (V): ");
    Serial.println(sensorVoltage);  // Print voltage with 3 decimal places
    Serial.print("CO2 Concentration (ppm): ");
    Serial.println(co2Concentration);
  }
  
  delay(100); // Adding a slight random delay to avoid message collision
}
