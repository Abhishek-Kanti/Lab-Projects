#include <HTTPClient.h>
#include <WiFi.h>
#include <DHT.h>

#define WIFI_SSID "SCLR_RM"
#define WIFI_PASS "sclrlab424"

String Web_App_URL = "https://script.google.com/macros/s/AKfycbxODYkqBiGt-a4clPFDZPzTXaza4jT7uUSWfpEUPS8RLZOopQ_ZLfwO7803a__ak-ZRMg/exec";  // Google script Web_App_URL.

#define DHT_Sensor_pin 25
#define Soil_Moisture_Sensor_pin1 34
#define Soil_Moisture_Sensor_pin2 35
#define Soil_Moisture_Sensor_pin3 32
const int gasSensorPin1 = 36;           // Analog pin for MG-811 sensor
const int gasSensorPin2 = 33;  
const int gasSensorPin3 = 39;   

#define DHTTYPE DHT22
DHT dht(DHT_Sensor_pin, DHTTYPE);

//Calibration variables (replace with your calculated values)
const float voltageToCO2Factor = -0.12;  // Calculated slope (m)
const float zeroPointCO2 = 700;       // Calculated intercept (b)

float wet1=1318.00, dry1=2544.00;
float wet2=193.00, dry2=476.00;
float wet3=203.00, dry3=477.00;

float calculateCO2Concentration(float sensorVoltage) {
  float co2Concentration = (sensorVoltage * voltageToCO2Factor) + zeroPointCO2;
  return co2Concentration;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(DHT_Sensor_pin, INPUT);
  pinMode(gasSensorPin1, INPUT);
  pinMode(gasSensorPin2, INPUT);
  pinMode(gasSensorPin3, INPUT);
  
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  //--------------------------------------------------------------------------MQ811------------------------------------------------------------------------------------------
  
  int rawGasValue1 = analogRead(gasSensorPin1);  // Raw analog reading
  int rawGasValue2 = analogRead(gasSensorPin2);  // Raw analog reading
  int rawGasValue3 = analogRead(gasSensorPin3);  // Raw analog reading

  Serial.println(" ");
  Serial.print("rawGasValue1 = ");
  Serial.print(rawGasValue1);
  Serial.print(" | ");
  Serial.print("rawGasValue2 = ");
  Serial.print(rawGasValue2);
  Serial.print(" | ");
  Serial.print("rawGasValue3 = ");
  Serial.println(rawGasValue3);

  //  int rawGasValue1 = 2000;  // Raw analog reading
  //  int rawGasValue2 = 4000;  // Raw analog reading
  //  int rawGasValue3 = 5000;  // Raw analog reading
   
   float sensorVoltage1 = rawGasValue1 * (3300 / 1024.0);  // Convert ADC value to voltage (assuming 3.3V reference)
   float sensorVoltage2 = rawGasValue2 * (3300 / 1024.0);  // Convert ADC value to voltage (assuming 3.3V reference)
   float sensorVoltage3 = rawGasValue3 * (3300 / 1024.0);  // Convert ADC value to voltage (assuming 3.3V reference)
   
   float gas1 = calculateCO2Concentration(sensorVoltage1);
   float gas2 = calculateCO2Concentration(sensorVoltage2);
   float gas3 = calculateCO2Concentration(sensorVoltage3);

  //-----------------------------------------------------------------------SoilMoisture---------------------------------------------------------------------------------------

  float rawSoilMoisture1 = analogRead(Soil_Moisture_Sensor_pin1);
  float rawSoilMoisture2 = analogRead(Soil_Moisture_Sensor_pin2);
  float rawSoilMoisture3 = analogRead(Soil_Moisture_Sensor_pin3);

  Serial.println(" ");
  Serial.print("rawSoilMoisture1 = ");
  Serial.print(rawSoilMoisture1);
  Serial.print(" | ");
  Serial.print("rawSoilMoisture2 = ");
  Serial.print(rawSoilMoisture2);
  Serial.print(" | ");
  Serial.print("rawSoilMoisture3 = ");
  Serial.println(rawSoilMoisture3);

  //  int rawSoilMoisture1 = 1318;
  //  int rawSoilMoisture2 = 476;
  //  int rawSoilMoisture3 = 300;

   float SoilMoisture1 = map(rawSoilMoisture1, wet1, dry1, 100, 0);
   if(SoilMoisture1 > 100) SoilMoisture1=100;
   if(SoilMoisture1 < 0) SoilMoisture1=0;
   
   float SoilMoisture2 = map(rawSoilMoisture2, wet2, dry2, 100, 0);
   if(SoilMoisture2 > 100) SoilMoisture2=100;
   if(SoilMoisture2 < 0) SoilMoisture2=0;
   
   float SoilMoisture3 = map(rawSoilMoisture3, wet3, dry3, 100, 0);
   if(SoilMoisture3 > 100) SoilMoisture3=100;
   if(SoilMoisture3 < 0) SoilMoisture3=0;

  //---------------------------------------------------------------------------DHT--------------------------------------------------------------------------------------------

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.println(" ");
  Serial.print("h = ");
  Serial.print(h);
  Serial.print(" | ");
  Serial.print("t = ");
  Serial.println(t);


  //  float h = 80.00;
  //  float t = 28.00;

   String temp, humd;

   if(isnan(h)){
     humd = ""; 
   }
   else humd = String(h);

   if(isnan(t)){
     temp = ""; 
   }
   else temp = String(t);

  //-------------------------------------------------------------------Writing Data to Google Sheets--------------------------------------------------------------------------

    if (WiFi.status() == WL_CONNECTED) {
      
      // Create a URL for sending or writing data to Google Sheets.
      String Send_Data_URL = Web_App_URL + "?sts=write";
      Send_Data_URL += "&temp=" + temp;
      Send_Data_URL += "&humd=" + humd;
      Send_Data_URL += "&gas1=" + String(gas1);
      Send_Data_URL += "&gas2=" + String(gas2);
      Send_Data_URL += "&gas3=" + String(gas3);
      Send_Data_URL += "&soil1=" + String(SoilMoisture1);
      Send_Data_URL += "&soil2=" + String(SoilMoisture2);
      Send_Data_URL += "&soil3=" + String(SoilMoisture3);
      
      Serial.println();
      Serial.println("-------------");
      Serial.println("Send data to Google Spreadsheet...");
      Serial.print("URL : ");
      Serial.println(Send_Data_URL);

      // Initialize HTTPClient as "http".
      HTTPClient http;
  
      // HTTP GET Request.
      http.begin(Send_Data_URL.c_str());
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  
      // Gets the HTTP status code.
      int httpCode = http.GET(); 
      Serial.print("HTTP Status Code : ");
      Serial.println(httpCode);
  
      // Getting response from google sheets.
      String payload;
      if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload : " + payload);    
      }
      
      http.end();  
  }

  delay(15000);
}
