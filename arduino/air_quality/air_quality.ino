

#include "config.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient clientWifi;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("starting");

  screenInit();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.printDiag(Serial);

  IPAddress ip = WiFi.localIP();
  char buf[20];
  sprintf(buf, "IP:%d", ip[3]);
  showText(buf, 0, 19, 2, true);

  mqttInit();
  pmsInit();
  outputInit();
}

void loop() {
  pmsLoop();
  mqttConnectionLoop();
  outputLoop();
}

