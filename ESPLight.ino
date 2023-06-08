#include <Wire.h>
#include "SSD1306.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_GFX.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SDA_PIN 5
#define SCL_PIN 4
#define SSD_ADDRESS 0x3c

String ipStr;
int number = 0;

SSD1306  display(SSD_ADDRESS, SDA_PIN, SCL_PIN);
WiFiClientSecure secureClient;
WiFiClient client;

const char* ssid       = "Z.A. Studios @ 2.4GHz";
const char* password   = "lastkiss";

String payload;

void setup() {
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  connectWiFi();
}

void loop() {
  display.clear();
  HTTPClient http;
  http.begin("https://www.bungie.net/Platform/Destiny2/3/Profile/4611686018496049651/?components=200");
  http.addHeader("X-API-Key", "ead5c42f08b74118b4679b62ce7e38e7");
  int httpCode = http.GET();
  String payload = http.getString();
  String input = http.getString();
  StaticJsonDocument<80> filter;
  filter["Response"]["characters"]["data"]["2305843009553216548"]["light"] = true;

  StaticJsonDocument<192> doc;

  DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  int Response_characters_data_2305843009553216548_light = doc["Response"]["characters"]["data"]["2305843009553216548"]["light"];
  String lightLevel = String(Response_characters_data_2305843009553216548_light);
  Serial.println( Response_characters_data_2305843009553216548_light);
  display.setFont(Lato_Black_35);
  display.drawString(0, 5, lightLevel);
  display.display();
  delay(60000);
}

void connectWiFi(void)
{
  uint32_t notConnectedCounter = 0;
  int i;
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  display.drawString(2, 10, "Connecting to: ");
  display.drawString(1, 25, String(ssid));
  display.display();
  WiFi.begin(ssid, password);
  while ((WiFi.status() != WL_CONNECTED) && i < 100)
  {

    i ++;
    delay(500);
    Serial.print(".");
    notConnectedCounter++;
    if (i >= 100) { // Reset board if not connected after 15s
      Serial.println("Resetting due to Wifi not connecting...");
      ESP.restart();
    }
    display.drawString((3 + i * 2), 35, "." );
    display.display();
  }
  if ( WiFi.status() == WL_CONNECTED)
  {
    IPAddress ip = WiFi.localIP();
    ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "WiFi Connected!");
    display.drawString(0, 25, ipStr);
    Serial.println("");
    Serial.println("WiFi Connedcted.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Subnet Mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());
    display.display();
  }
  else
  {
    ipStr = "NOT CONNECTED";
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0,  "WiFi not connected.");
    display.drawString(0, 25, ipStr);
    Serial.println("");
    Serial.println("WiFi not connected.");
    display.display();
  }
  delay(1000);
}
