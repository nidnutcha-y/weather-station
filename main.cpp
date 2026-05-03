#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

#include "config.h"
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* mqtt_server = MQTT_SERVER;

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT...");
    if (client.connect("ESP32_Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!aht.begin()) {
    Serial.println("AHT20 not found!");
    while (1) delay(10);
  }

  if (!bmp.begin(0x76)) { 
    Serial.println("BMP280 not found!");
    while (1) delay(10);
  }

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sensors_event_t humidity_event, temp_event;
  aht.getEvent(&humidity_event, &temp_event);

  float temperature = temp_event.temperature;
  float humidity = humidity_event.relative_humidity;

  float pressure = bmp.readPressure() / 100.0F;  
  float altitude = bmp.readAltitude(1013.25);  

  if (isnan(temperature) || isnan(humidity) || isnan(pressure)) {
    Serial.println("Sensor read error!");
    return;
  }

  String payload = "{\"temperature\":" + String(temperature, 1) +
                   ",\"humidity\":" + String(humidity, 1) +
                   ",\"pressure\":" + String(pressure, 1) +
                   ",\"altitude\":" + String(altitude, 1) + "}";

  client.publish("weather/data", payload.c_str());

  Serial.print("[");
  Serial.print(millis() / 1000);
  Serial.print("s] Temp: ");
  Serial.print(temperature, 1);
  Serial.print("°C | Humidity: ");
  Serial.print(humidity, 1);
  Serial.print("% | Pressure: ");
  Serial.print(pressure, 1);
  Serial.print("hPa | Altitude: ");
  Serial.print(altitude, 1);
  Serial.println("m");

  delay(5000);
}
