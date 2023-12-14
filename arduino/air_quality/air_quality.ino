

#include "config.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>


ESP8266WiFiMulti wifiMulti;
WiFiClient clientWifi;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("starting");

  ledInit() ;
  screenInit();

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(ssid, password);
  wifiMulti.addAP(ssid2, password2);

  Serial.println("Connecting Wifi...");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    delay(500);
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
  wifiMulti.run();
  pmsLoop();
  mqttConnectionLoop();
  outputLoop();
}

