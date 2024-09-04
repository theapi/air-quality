

#include "config.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include "UdpAQIPayload.h"

// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <U8g2_for_Adafruit_GFX.h>

GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS=D8*/ 5, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
ESP8266WiFiMulti wifiMulti;
WiFiClient clientWifi;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
theapi::UdpAQIPayload aqi_payload = theapi::UdpAQIPayload();
IPAddress ipMulti(239, 0, 0, 47);
unsigned int portMulti = 12345;
char incomingPacket[255];  // buffer for incoming packets

unsigned long currentMillis = 0;
const long sensorTimeout = 600000; // 10 minutes
unsigned long sensorAtime = 0;
unsigned long sensorBtime = 0;
unsigned long sensorCtime = 0;

uint16_t bg = GxEPD_WHITE;
uint16_t fg = GxEPD_BLACK;
byte pixelHeight = 28;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("starting");


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

  Udp.beginMulticast(WiFi.localIP(), ipMulti, portMulti);
  Serial.printf("Now listening to IP %s, UDP port %d\n", ipMulti.toString().c_str(), portMulti);

  //mqttInit();

  display.init();
  u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX
  display.setRotation(1);

  u8g2Fonts.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);            // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);         // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);         // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_logisoso28_tn);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall

  Serial.println(display.width());
  Serial.println(display.height());

  helloWorld();

  Serial.println("setup done");
}

void loop() {
  //mqttConnectionLoop();


  currentMillis = millis();
  // Check for udp data.
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)  {
      incomingPacket[len] = 0;
    }
    //Serial.printf("UDP packet contents: %s\n", incomingPacket);
    // Check for payload signifier;
    if (incomingPacket[0] == '\t') {
      uint8_t payload_buf[aqi_payload.size()];
      memcpy(payload_buf, incomingPacket + 1, aqi_payload.size());
      aqi_payload.unserialize(payload_buf);
      if (aqi_payload.getMsgType() == 10) {
        Serial.print(aqi_payload.getMsgType()); Serial.print(", ");
        Serial.print(aqi_payload.getMsgId()); Serial.print(", ");
        Serial.print(aqi_payload.getDeviceId()); Serial.print(", ");
        Serial.print(aqi_payload.getAqi()); Serial.print(", ");
        Serial.println();

        switch (aqi_payload.getDeviceId()) {
          case sensorA:
            diplaySensorA(aqi_payload.getAqi(), true);
            break;
          case sensorB:
            diplaySensorB(aqi_payload.getAqi(), true);
            break;
          case sensorC:
            diplaySensorC(aqi_payload.getAqi(), true);
            break;
          default:
            break;
        }

      }
    }
  } else {
    if (currentMillis - sensorAtime >= sensorTimeout) {
      diplaySensorA(0, false);
    }

    if (currentMillis - sensorBtime >= sensorTimeout) {
      diplaySensorB(0, false);
    }

    if (currentMillis - sensorCtime >= sensorTimeout) {
      diplaySensorC(0, false);
    }
  }
}

void displayAqi() {

  uint16_t x = 8;
  uint16_t y = pixelHeight + 5;
  //uint16_t y = display.height() - 20;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(bg);
    u8g2Fonts.setCursor(x, y); // start writing at this position
    u8g2Fonts.print(aqi_payload.getAqi());
    //    y = y + pixelHeight + 5;
    //    u8g2Fonts.setCursor(x, y);
    //    u8g2Fonts.print("88:88");
    //    y = y + pixelHeight + 5;
    //    u8g2Fonts.setCursor(x, y);
    //    u8g2Fonts.print("1234");
  }
  while (display.nextPage());
}

void diplaySensorA(uint16_t aqi, bool current) {
  uint16_t x = 8;
  uint16_t y = pixelHeight + 5;
  display.firstPage();
  do
  {
    display.setPartialWindow(0, 0, display.width(), pixelHeight);

    display.fillScreen(bg);
    u8g2Fonts.setCursor(x, y); // start writing at this position
    if (current) {
      u8g2Fonts.print(aqi);
    } else {
      u8g2Fonts.print("-");
    }
  }
  while (display.nextPage());
}

void diplaySensorB(uint16_t aqi, bool current) {
  uint16_t x = 8;
  uint16_t y = pixelHeight + 5 + pixelHeight + 5;
  display.firstPage();
  do
  {
    display.setPartialWindow(0, pixelHeight + 5, display.width(), pixelHeight + 5);

    display.fillScreen(bg);
    u8g2Fonts.setCursor(x, y); // start writing at this position
    if (current) {
      u8g2Fonts.print(aqi);
    } else {
      u8g2Fonts.print("-");
    }
  }
  while (display.nextPage());
}

void diplaySensorC(uint16_t aqi, bool current) {
  uint16_t x = 8;
  uint16_t y = pixelHeight + 5 + pixelHeight + 5 + pixelHeight + 5;
  display.firstPage();
  do
  {
    display.setPartialWindow(0, pixelHeight + 5 + pixelHeight + 5, display.width(), pixelHeight + 5 + pixelHeight + 5);

    display.fillScreen(bg);
    u8g2Fonts.setCursor(x, y); // start writing at this position
    if (current) {
      u8g2Fonts.print(aqi);
    } else {
      u8g2Fonts.print("-");
    }
  }
  while (display.nextPage());
}

void helloWorld() {

  uint16_t x = 8;
  uint16_t y = pixelHeight + 5;
  //uint16_t y = display.height() - 20;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(bg);
    u8g2Fonts.setCursor(x, y); // start writing at this position
    u8g2Fonts.print("-");
    y = y + pixelHeight + 5;
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.print("-");
    y = y + pixelHeight + 5;
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.print("-");
  }
  while (display.nextPage());
}


