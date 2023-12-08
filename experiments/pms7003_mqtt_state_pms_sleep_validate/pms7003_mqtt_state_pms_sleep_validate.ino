

#include "config.h"
#include <ESP8266WiFi.h>

WiFiClient clientWifi;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("starting");

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

  mqttInit();
  pmsInit();
  outputInit();
}

void loop() {
  pmsLoop();
  mqttConnectionLoop();
  outputLoop();
}

