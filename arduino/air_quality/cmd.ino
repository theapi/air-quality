
enum DisplayState {
  DISPLAY_STATE_OFF,
  DISPLAY_STATE_AQI,
  DISPLAY_STATE_GRAPH,
  DISPLAY_STATE_PM_VALUES
};
DisplayState displayState = DISPLAY_STATE_GRAPH;

enum LedState {
  LED_STATE_OFF,
  LED_STATE_ON,
};
LedState ledState = LED_STATE_ON;

void handleCommand(char cmd, char param) {
  switch (cmd) {
    case 'D':
      switch (param) {
        case 'o':
          Serial.println("display: off");
          if (displayState != DISPLAY_STATE_OFF) {
            displayState = DISPLAY_STATE_OFF;
          }
          break;
        case 'a':
          Serial.println("display: AQI");
          if (displayState != DISPLAY_STATE_AQI) {
            displayState = DISPLAY_STATE_AQI;
          }
          break;
        case 'g':
          Serial.println("display: graph");
          if (displayState != DISPLAY_STATE_GRAPH) {
            displayState = DISPLAY_STATE_GRAPH;
          }
          break;
        case 'v':
          Serial.println("display: pm values");
          if (displayState != DISPLAY_STATE_PM_VALUES) {
            displayState = DISPLAY_STATE_PM_VALUES;
          }
          break;
        default:
          break;
      }
      displayScreen();
      break;
    case 'L':
      switch (param) {
        case 't':
          Serial.println("led: toggle");
          if (ledState != LED_STATE_OFF) {
            ledState = LED_STATE_OFF;
          } else {
            ledState = LED_STATE_ON;
          }
          break;
        default:
          break;
      }
      displayLed();
      break;
    default:
      break;
  }
}

