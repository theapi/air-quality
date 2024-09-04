
// https://arduinojson.org
#include <ArduinoJson.h>

enum DisplayState {
  DISPLAY_STATE_OFF,
  DISPLAY_STATE_AQI,
  DISPLAY_STATE_GRAPH,
  DISPLAY_STATE_PM_VALUES
};
DisplayState displayState = DISPLAY_STATE_GRAPH;

char sensor[10];
char sensorVal[10];
JsonDocument doc;

void cmdHandle(byte* payload, uint8_t length) {

  // {"device":"d38f88","aqi":25}
  char characters[length];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    characters[i] = (char)payload[i];
  }
  Serial.println();

  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    Serial.println();
  }

  const char* device = doc["device"];
  const int aqi = doc["aqi"];
  Serial.println(device);
  Serial.println(aqi);


  //  switch (cmd) {
  //    case 'D':
  //      // D:d38f88:29
  //      for (int i = 0; i < length; i++) {
  //        Serial.print((char)payload[i]);
  //        if (i > 2 && i < 9) {
  //          sensor[i] = (char)payload[i];
  //        }
  //        if (i == 9) {
  //          sensor[i] = '\0'; //terminate string
  //        }
  //        if (i > 9 && i < 12) {
  //          // bad with 2 character numbers :(
  //          sensorVal[i] = (char)payload[i];
  //        }
  //        if (i == 12) {
  //          sensorVal[i] = '\0'; //terminate string
  //        }
  //      }
  //      Serial.println(sensor);
  //      Serial.println(sensorVal);
  //      break;
  //    default:
  //      break;
  //  }
}

