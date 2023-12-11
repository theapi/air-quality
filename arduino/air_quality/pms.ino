
#include <SoftwareSerial.h>

const byte rxPin = D6; // from the sensor
const byte txPin = D7;
SoftwareSerial pmsSerial(rxPin, txPin);

const long pmsInterval = 60000;
const long pmsWakeInterval = 30000;
unsigned long pmsPreviousRead = 0;

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
struct pms5003data pmsData;

enum PmsState {
  PMS_STATE_SETUP,
  PMS_STATE_WAITING_FOR_DATA,
  PMS_STATE_DATA_RECEIVED,
  PMS_STATE_SLEEPING,
  PMS_STATE_WAKING
};
PmsState pmsState = PMS_STATE_SETUP;

void pmsInit() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  // sensor baud rate is 9600
  pmsSerial.begin(9600);
  pmsActiveMode(&pmsSerial);
  pmsWakeUp(&pmsSerial);
  pmsState = PMS_STATE_WAITING_FOR_DATA;
}

void pmsLoop() {
  switch (pmsState) {
    case PMS_STATE_SETUP:
      pmsInit();
      break;
    case PMS_STATE_WAITING_FOR_DATA:
      if (readPMSdata(&pmsSerial)) {
        int aqival = calcAQI25(pmsData.pm25_env);

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

        pmsPreviousRead = millis();
        pmsState = PMS_STATE_DATA_RECEIVED;
      }
      break;
    case PMS_STATE_DATA_RECEIVED:
      {
        pmsSleep(&pmsSerial);
        pmsState = PMS_STATE_SLEEPING;
        break;
      }
    //      {
    //        unsigned long currentMillis = millis();
    //        if (currentMillis - pmsPreviousRead >= pmsInterval) {
    //          pmsState = PMS_STATE_WAITING_FOR_DATA;
    //        }
    //        break;
    //      }
    case PMS_STATE_SLEEPING:
      {
        unsigned long currentMillis = millis();
        if (currentMillis - pmsPreviousRead >= pmsInterval) {
          pmsWakeUp(&pmsSerial);
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

uint16_t pmsGetPm10Env() {
  return pmsData.pm10_env;
}

uint16_t pmsGetPm25Env() {
  return pmsData.pm25_env;
}

uint16_t pmsGetPm100Env() {
  return pmsData.pm100_env;
}

int pmsGetAqi() {
  return calcAQI25(pmsData.pm25_env);
}

unsigned long pmsGetLastReadingTime() {
  return pmsPreviousRead;
}

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
  memcpy((void *)&pmsData, (void *)buffer_u16, 30);

  if (sum != pmsData.checksum) {
    return false;
  }
  // success!
  return true;
}

// Standby mode. For low power consumption and prolong the life of the sensor.
void pmsSleep(Stream *s) {
  uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 };
  s->write(command, sizeof(command));
}

// Operating mode. Stable data should be got at least 30 seconds after the sensor wakeup from the sleep mode because of the fan's performance.
void pmsWakeUp(Stream *s) {
  uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74 };
  s->write(command, sizeof(command));
}

// Active mode. Default mode after power up. In this mode sensor would send serial data to the host automatically.
void pmsActiveMode(Stream *s) {
  uint8_t command[] = { 0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71 };
  s->write(command, sizeof(command));
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
//String make_aqi_words(float aqival) {
//  if (aqival <= 50) {
//    return "Good";
//  }
//  if (aqival <= 100) {
//    return "Moderate";
//  }
//  if (aqival <= 150) {
//    return "Unhealthy for Sensitive Groups";
//  }
//  if (aqival <= 200) {
//    return "Unhealthy";
//  }
//  if (aqival <= 300) {
//    return "Very Unhealthy";
//  }
//  if (aqival > 300) {
//    return "Hazardous";
//  }
//  return "Error";
//}

