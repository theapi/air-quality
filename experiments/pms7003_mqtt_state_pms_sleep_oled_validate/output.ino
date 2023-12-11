
enum OutputState {
  OUTPUT_STATE_SETUP,
  OUTPUT_STATE_READY,
  OUTPUT_STATE_SENDING,
};
OutputState outputState = OUTPUT_STATE_SETUP;

unsigned long outputLastProcessed = 0;

void outputInit() {
  outputState = OUTPUT_STATE_SETUP;
}

void outputLoop() {
  switch (outputState) {
    case OUTPUT_STATE_SETUP:
      outputState = OUTPUT_STATE_READY;
      break;
    case OUTPUT_STATE_READY:
      {
        unsigned long outputLastReading = pmsGetLastReadingTime();
        if (outputLastReading > outputLastProcessed) {
          outputLastProcessed = outputLastReading;
          outputState = OUTPUT_STATE_SENDING;
        }
        break;
      }

    case OUTPUT_STATE_SENDING:
      {
        display.clearDisplay();
        char buf[20];
        int aqival = pmsGetAqi();
        sprintf(buf, "%d", aqival);

        int x = 57;
        if (aqival > 9) {
          x = 45;
        } 
        if (aqival > 99) {
          x = 33;
        }
        showText(buf, x, 0, 4, true);
        
        mqttPublishReport();
        outputState = OUTPUT_STATE_READY;
        break;
      }
    default:
      break;
  }
}


