#ifndef CONFIG_H
#define CONFIG_H

/* ----------------- General config -------------------------------- */
/* WiFi */
const char* ssid                  = "";     // Your WiFi SSID
const char* password              = "";     // Your WiFi password
const char* ssid2      = "";      // Your alternate WiFi SSID
const char* password2  = ""; // Your alternate WiFi password

/* MQTT */
const char* mqtt_broker           = "192.168.0.32"; // IP address of your MQTT broker
const char* mqtt_username         = "";              // Your MQTT username
const char* mqtt_password         = "";              // Your MQTT password
const char* status_topic          = "events";        // MQTT topic to report startup

const uint32_t sensorA = 13587140;
const uint32_t sensorB = 13865611;
const uint32_t sensorC = 3; //@todo get the real value

#endif

