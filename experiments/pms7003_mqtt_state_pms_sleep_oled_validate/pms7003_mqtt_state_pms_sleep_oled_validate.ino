

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

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
  }

  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(1);
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text

  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setCursor(0, 0);     // Start at top-left corner

  char screen_buffer[20];
  sprintf(screen_buffer, "%x", ESP.getChipId());
  showText(screen_buffer, 0, 0, 2, true);

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
  sprintf(screen_buffer, "IP:%d", ip[3]);
  showText(screen_buffer, 0, 19, 2, true);

  mqttInit();
  pmsInit();
  outputInit();
}

void loop() {
  pmsLoop();
  mqttConnectionLoop();
  outputLoop();
}

void showText(char* text, int x, int y, int size, boolean d) {
  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(text);
  if (d) {
    display.display();
  }
}

