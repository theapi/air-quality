
// https://github.com/knolleary/pubsubclient/tree/master
#include <PubSubClient.h>

PubSubClient clientMqtt(clientWifi);

const long mqqtReconnectTryInterval = 5000;
unsigned long mqttPreviousReconnectTry = 0;

char g_mqtt_message_buffer[125];
char g_mqtt_topic_csv[50];
char g_command_topic[50];

enum MqttState {
  MQTT_STATE_SETUP,
  MQTT_STATE_CONNECTING,
  MQTT_STATE_CONNECT_FAILED,
  MQTT_STATE_CONNECT_SUCCESS
};
MqttState mqttState = MQTT_STATE_SETUP;


void mqttConnectionLoop() {
  clientMqtt.loop();
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
          clientMqtt.subscribe(g_command_topic);
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
  sprintf(g_mqtt_topic_csv, "sensor/%x/csv", ESP.getChipId());
  sprintf(g_command_topic, "sensor/%x/cmd", ESP.getChipId());
  Serial.println(g_command_topic);
  clientMqtt.setServer(mqtt_broker, 1883);
  clientMqtt.setCallback(mqttCallback);
}

void mqttPublishReport() {
  int aqival = pmsGetAqi();
  sprintf(g_mqtt_message_buffer, "%d,%d,%d,%d", aqival, pmsGetPm25Env(), pmsGetPm100Env(), pmsGetPm10Env());
  clientMqtt.publish(g_mqtt_topic_csv, g_mqtt_message_buffer);
}

void mqttCallback(char* topic, byte* payload, uint8_t length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  handleCommand(payload[0], payload[1]);
}


