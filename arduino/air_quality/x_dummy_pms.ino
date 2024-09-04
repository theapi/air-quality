

void dummy_pmsLoop() {
  switch (pmsState) {
    case PMS_STATE_SETUP:
      pmsInit();
      break;
    case PMS_STATE_WAITING_FOR_DATA:
      {
        int aqival = calcAQI25(pmsData.pm25_env);

        Serial.println();
        Serial.println("!!! Dummy PMS Data !!!");

        Serial.print("PM1.0 standard: ");
        Serial.print(pmsData.pm10_standard);
        Serial.print(" ug/m3");
        Serial.print(" environment: ");
        Serial.print(pmsData.pm10_env);
        Serial.print(" ug/m3");

        Serial.println();

        Serial.print("PM2.5 standard: ");
        Serial.print(pmsData.pm25_standard);
        Serial.print(" ug/m3");
        Serial.print(" environment: ");
        Serial.print(pmsData.pm25_env);
        Serial.print(" ug/m3");

        Serial.println();

        Serial.print("PM10 standard: ");
        Serial.print(pmsData.pm100_standard);
        Serial.print(" ug/m3");
        Serial.print(" environment: ");
        Serial.print(pmsData.pm100_env);
        Serial.print(" ug/m3");

        Serial.println();

        Serial.print("AQI: ");
        Serial.print(aqival);
        Serial.println();
        Serial.println("!!! End dummy PMS data !!!");

        pmsPreviousRead = millis();
        pmsState = PMS_STATE_DATA_RECEIVED;
        break;
      }
    case PMS_STATE_DATA_RECEIVED:
      {
        pmsState = PMS_STATE_SLEEPING;
        break;
      }
    case PMS_STATE_SLEEPING:
      {
        unsigned long currentMillis = millis();
        if (currentMillis - pmsPreviousRead >= pmsInterval) {
          pmsState = PMS_STATE_WAKING;
        }
        break;
      }
      break;
    case PMS_STATE_WAKING:
      {
        unsigned long currentMillis = millis();
        if (currentMillis - pmsPreviousRead >= pmsWakeInterval) {
          pmsState = PMS_STATE_WAITING_FOR_DATA;
        }
      }

      break;
    default:
      break;
  }
}

