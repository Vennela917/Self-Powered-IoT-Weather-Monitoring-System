#include <Wire.h> 
#include <Adafruit_Sensor.h> 
#include <Adafruit_BMP280.h> 
#include <DHT.h> 
#include <ESP8266WiFi.h> 
#include <ThingSpeak.h> 

// Wi-Fi credentials 
const char* ssid = "Empty"; 
const char* password = "lakshmi1272"; 

// ThingSpeak channel details 
unsigned long channelID = 2888912; 
const char* writeAPIKey = "KSNEHKSHJOPIGEZ1"; 

// Create sensor objects 
Adafruit_BMP280 bmp; 
DHT dht(D4, DHT11); // DHT11 sensor connected to GPIO2 (D4) 

// Rain sensor setup 
#define RAIN_SENSOR_PIN A0 // Analog pin connected to AO of the rain sensor 

WiFiClient client; 

void setup() { 
  Serial.begin(115200); 

  // Connect to Wi-Fi 
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000); 
    Serial.println("Connecting to WiFi..."); 
  } 
  Serial.println("Connected to WiFi"); 

  // Initialize BMP280 sensor 
  if (!bmp.begin(0x76)) { 
    Serial.println("Could not find a valid BMP280 sensor, check wiring!"); 
    while (1); 
  } 

  // Initialize DHT11 sensor 
  dht.begin(); 

  // Initialize ThingSpeak 
  ThingSpeak.begin(client); 
} 

void loop() { 

  // Read BMP280 data 
  float temperatureBMP = bmp.readTemperature(); 
  float pressureBMP = bmp.readPressure() / 100.0F; // Pressure in hPa 

  // Read DHT11 data 
  delay(2000); // DHT11 needs at least 2 seconds between reads 
  float humidityDHT = dht.readHumidity(); 
  float temperatureDHT = dht.readTemperature(); 

  // Read Rain sensor data 
  int rainValue = analogRead(RAIN_SENSOR_PIN); 

  // Print sensor values to Serial Monitor 
  Serial.print("BMP280 Temperature: "); 
  Serial.print(temperatureBMP); 
  Serial.println(" *C"); 

  Serial.print("BMP280 Pressure: "); 
  Serial.print(pressureBMP); 
  Serial.println(" hPa"); 

  Serial.print("DHT11 Humidity: "); 
  Serial.print(humidityDHT); 
  Serial.println(" %"); 

  Serial.print("DHT11 Temperature: "); 
  Serial.print(temperatureDHT); 
  Serial.println(" *C"); 

  Serial.print("Rain Sensor Value: "); 
  Serial.println(rainValue); 

  // Update ThingSpeak fields 
  ThingSpeak.setField(1, temperatureBMP);    // Field 1: BMP280 Temperature 
  ThingSpeak.setField(2, pressureBMP);       // Field 2: BMP280 Pressure 
  ThingSpeak.setField(3, humidityDHT);       // Field 3: DHT11 Humidity 
  ThingSpeak.setField(4, temperatureDHT);    // Field 4: DHT11 Temperature 
  ThingSpeak.setField(5, rainValue);         // Field 5: Rain Sensor Value 

  // Write data to ThingSpeak 
  int responseCode = ThingSpeak.writeFields(channelID, writeAPIKey); 

  if (responseCode == 200) { 
    Serial.println("Data successfully sent to ThingSpeak"); 
  } else { 
    Serial.print("Error sending data to ThingSpeak. Response code: "); 
    Serial.println(responseCode); 
  } 

  // Wait before next reading 
  delay(20000); // ThingSpeak allows updates every 15 seconds 
}