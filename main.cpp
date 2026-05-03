#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include <WiFi.h>

#include "config.h"
const char* ssid = "Your wifi name";
const char* password = "Your wifi password";

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (!aht.begin()) {
    Serial.println("AHT20 not found!");
    while (1) delay(10);
  }

  if (!bmp.begin(0x76)) {
    if (!bmp.begin(0x77)) {
      Serial.println("BMP280 not found!");
      while (1) delay(10);
    }
  }
}

void loop() {
  sensors_event_t humidity_event, temp_event;
  aht.getEvent(&humidity_event, &temp_event);

  float temperature = temp_event.temperature;
  float humidity = humidity_event.relative_humidity;
  float pressure = bmp.readPressure() / 100.0F;
  float altitude = bmp.readAltitude(1013.25);

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
