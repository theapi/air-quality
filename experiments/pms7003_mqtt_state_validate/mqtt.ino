
#include <PubSubClient.h>

PubSubClient clientMqtt(clientWifi);

const long mqqtReconnectTryInterval = 5000;
unsigned long mqttPreviousReconnectTry = 0;

char g_mqtt_message_buffer[125];
char g_pm25_env_mqtt_topic[50];
char g_pm100_env_mqtt_topic[50];
char g_aqi_mqtt_topic[50];

enum MqttState {
  MQTT_STATE_SETUP,
  MQTT_STATE_CONNECTING,
  MQTT_STATE_CONNECT_FAILED,
  MQTT_STATE_CONNECT_SUCCESS
};
MqttState mqttState = MQTT_STATE_SETUP;


void mqttConnectionLoop() {
  switch (mqttState) {
    case MQTT_STATE_SETUP:
      mqttState = MQTT_STATE_CONNECTING;
      break;
    case MQTT_STATE_CONNECTING:
      {
        char mqtt_client_id[20];
        sprintf(mqtt_client_id, "esp8266-%x", ESP.getChipId());
        Serial.println("Attempting MQTT connection...");
        // Attempt to connect
        if (clientMqtt.connect(mqtt_client_id, mqtt_username, mqtt_password))
        {
          mqttState = MQTT_STATE_CONNECT_SUCCESS;
          Serial.println("MQTT connected");
          // Once connected, publish an announcement
          sprintf(g_mqtt_message_buffer, "Device %s starting up", mqtt_client_id);
          clientMqtt.publish(status_topic, g_mqtt_message_buffer);

          // Resubscribe
          //clientMqtt.subscribe(g_command_topic);
        } else {
          mqttPreviousReconnectTry = millis();
          mqttState = MQTT_STATE_CONNECT_FAILED;

          Serial.print("failed, rc=");
          Serial.print(clientMqtt.state());
          Serial.println(" trying again later");
        }
      }

      break;
    case MQTT_STATE_CONNECT_FAILED:
      {
        unsigned long currentMillis = millis();
        if (currentMillis - mqttPreviousReconnectTry >= mqqtReconnectTryInterval) {
          mqttState = MQTT_STATE_CONNECTING;
        }
        break;
      }
    case MQTT_STATE_CONNECT_SUCCESS:
      {
        if (WiFi.status() == WL_CONNECTED) {
          if (!clientMqtt.connected()) {
            mqttState = MQTT_STATE_CONNECTING;
          }
        }
        break;
      }
    default:
      break;
  }
}

void mqttInit() {
  sprintf(g_pm25_env_mqtt_topic, "tele/%x/PM25", ESP.getChipId());
  sprintf(g_pm100_env_mqtt_topic, "tele/%x/PM100", ESP.getChipId());
  sprintf(g_aqi_mqtt_topic, "tele/%x/AQI", ESP.getChipId());
  clientMqtt.setServer(mqtt_broker, 1883);
}

void mqttPublishReport() {
  sprintf(g_mqtt_message_buffer, "%d", pmsGetPm25Env());
  clientMqtt.publish(g_pm25_env_mqtt_topic, g_mqtt_message_buffer);
  sprintf(g_mqtt_message_buffer, "%d", pmsGetPm100Env());
  clientMqtt.publish(g_pm100_env_mqtt_topic, g_mqtt_message_buffer);

  int aqival = pmsGetAqi();
  sprintf(g_mqtt_message_buffer, "%d", aqival);
  clientMqtt.publish(g_aqi_mqtt_topic, g_mqtt_message_buffer);
}


