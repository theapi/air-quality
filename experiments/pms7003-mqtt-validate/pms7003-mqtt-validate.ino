
/*

   bool SoftwareSerial::isValidGPIOpin(int pin) {
   // Some GPIO pins are reserved by the system
   return (pin >= 0 && pin <= 5) || (pin >= 12 && pin <= 15);
  }
*/

#include "config.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>             // Required for MQTT
#include <SoftwareSerial.h>

const byte rxPin = D6; // from the sensor
const byte txPin = D7;

unsigned long previousMillis = 0;
const long interval = 10000;

SoftwareSerial pmsSerial(rxPin, txPin);
WiFiClient esp_client;
PubSubClient client(esp_client);

char g_mqtt_message_buffer[125];
char g_pm25_env_mqtt_topic[50];
char g_pm100_env_mqtt_topic[50];
char g_aqi_mqtt_topic[50];

void setup()   {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

Serial.setDebugOutput(true);
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

  sprintf(g_pm25_env_mqtt_topic,   "tele/%x/PM25",     ESP.getChipId());
  sprintf(g_pm100_env_mqtt_topic,   "tele/%x/PM100",     ESP.getChipId());
  sprintf(g_aqi_mqtt_topic,   "tele/%x/AQI",     ESP.getChipId());

  /* Set up the MQTT client */
  client.setServer(mqtt_broker, 1883);

  // sensor baud rate is 9600
  pmsSerial.begin(9600);
}

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;

void loop() {
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!client.connected())
    {
      reconnectMqtt();
    }
  }

//  unsigned long currentMillis = millis();
//  if (currentMillis - previousMillis >= interval) {
//    previousMillis = currentMillis;

//Serial.println("Waiting for sensor reading...");
    if (readPMSdata(&pmsSerial)) {
      int aqival = calcAQI25(data.pm25_env);
      String aqi_text = make_aqi_words(aqival);

      Serial.print("PM1.0 standard: ");
      Serial.print(data.pm10_standard);
      Serial.print(" ug/m3");
      Serial.print(" environment: ");
      Serial.print(data.pm10_env);
      Serial.print(" ug/m3");

      Serial.println();

      Serial.print("PM2.5 standard: ");
      Serial.print(data.pm25_standard);
      Serial.print(" ug/m3");
      Serial.print(" environment: ");
      Serial.print(data.pm25_env);
      Serial.print(" ug/m3");

      Serial.println();

      Serial.print("PM10 standard: ");
      Serial.print(data.pm100_standard);
      Serial.print(" ug/m3");
      Serial.print(" environment: ");
      Serial.print(data.pm100_env);
      Serial.print(" ug/m3");

      Serial.println();

      Serial.print("AQI: ");
      Serial.print(aqival);
      Serial.print(" ");
      Serial.print(aqi_text);
      Serial.println();

      //    String message_string;
      //message_string = String(data.pm25_env);
      //    message_string.toCharArray(g_mqtt_message_buffer, message_string.length() + 1);

      //    sprintf(g_mqtt_message_buffer, "PM2.5: %d ug/m3", data.pm25_env);
      //    client.publish(status_topic, g_mqtt_message_buffer);

if (WiFi.status() != WL_CONNECTED) {
      Serial.print("wifi status: ");
      Serial.println(WiFi.status());
      
      WiFi.printDiag(Serial);
}
      Serial.print("mqtt client connected: ");
      Serial.println(client.connected());
      Serial.print("mqtt client state: ");
      Serial.println(client.state());
      if (!client.connected())
      {
        reconnectMqtt();
      }


      sprintf(g_mqtt_message_buffer, "%d", data.pm25_env);
      client.publish(g_pm25_env_mqtt_topic, g_mqtt_message_buffer);
      sprintf(g_mqtt_message_buffer, "%d", data.pm100_env);
      client.publish(g_pm100_env_mqtt_topic, g_mqtt_message_buffer);
      sprintf(g_mqtt_message_buffer, "%d", aqival);
      client.publish(g_aqi_mqtt_topic, g_mqtt_message_buffer);


      delay(10000);  // yuck!
    }
  }

//}

boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }

  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}



int calcAQI25(int pm25) {

  // Uses formula AQI = ( (pobs - pmin) x (aqimax - aqimin) ) / (pmax - pmin)  + aqimin
  float pmin, pmax, amin, amax;

  if (pm25 <= 12) {
    pmin = 0; pmax = 12; amin = 0; amax = 50;            goto aqicalc;
  }
  if (pm25 <= 35.5) {
    pmin = 12; pmax = 35.5; amin = 50; amax = 100;        goto aqicalc;
  }
  if (pm25 <= 55.5) {
    pmin = 35.5; pmax = 55.5; amin = 100; amax = 150;     goto aqicalc;
  }
  if (pm25 <= 150.5) {
    pmin = 55.5; pmax = 150.5; amin = 150; amax = 200;    goto aqicalc;
  }
  if (pm25 <= 250.5) {
    pmin = 150.5; pmax = 250.5; amin = 200; amax = 300;    goto aqicalc;
  }
  if (pm25 <= 350.5) {
    pmin = 250.5; pmax = 350.5; amin = 300; amax = 400;    goto aqicalc;
  }
  if (pm25 <= 500.5) {
    pmin = 350.5; pmax = 500.5; amin = 400; amax = 500;    goto aqicalc;
  } else {
    return 999;
  }

aqicalc:
  float aqi = ( ((pm25 - pmin) * (amax - amin))  / (pmax - pmin) ) + amin;
  return aqi;

}

// https://aqicn.org/calculator
String make_aqi_words(float aqival) {
  if (aqival <= 50) {
    return "Good";
  }
  if (aqival <= 100) {
    return "Moderate";
  }
  if (aqival <= 150) {
    return "Unhealthy for Sensitive Groups";
  }
  if (aqival <= 200) {
    return "Unhealthy";
  }
  if (aqival <= 300) {
    return "Very Unhealthy";
  }
  if (aqival > 300) {
    return "Hazardous";
  }
  return "Error";
}

/**
  Reconnect to MQTT broker, and publish a notification to the status topic
*/
void reconnectMqtt() {
  char mqtt_client_id[20];
  sprintf(mqtt_client_id, "esp8266-%x", ESP.getChipId());

  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_client_id, mqtt_username, mqtt_password))
    {
      //Serial.println("connected");
      // Once connected, publish an announcement
      sprintf(g_mqtt_message_buffer, "Device %s starting up", mqtt_client_id);
      client.publish(status_topic, g_mqtt_message_buffer);

      sprintf(g_mqtt_message_buffer, "g_pm25_env_mqtt_topic: %s", g_pm25_env_mqtt_topic);
      client.publish(status_topic, g_mqtt_message_buffer);

      // Resubscribe
      //client.subscribe(g_command_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      //@TODO continue taking reading even when no mqtt or wifi, when it has a screen

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


