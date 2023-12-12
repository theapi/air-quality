
enum CMDState {
  CMD_STATE_SCREEN_OFF,
  CMD_STATE_AQI,
  CMD_STATE_GRAPH,
  CMD_STATE_PM_VALUES
};
CMDState cmdState = CMD_STATE_GRAPH;

void handleCommand(char cmd, char param) {
  switch (cmd) {
    case 'D':
      switch (param) {
        case 'o':
          Serial.println("display: off");
          if (cmdState != CMD_STATE_SCREEN_OFF) {
            cmdState = CMD_STATE_SCREEN_OFF;
          }
          break;
        case 'a':
          Serial.println("display: AQI");
          if (cmdState != CMD_STATE_AQI) {
            cmdState = CMD_STATE_AQI;
          }
          break;
        case 'g':
          Serial.println("display: graph");
          if (cmdState != CMD_STATE_GRAPH) {
            cmdState = CMD_STATE_GRAPH;
          }
          break;
        case 'v':
          Serial.println("display: pm values");
          if (cmdState != CMD_STATE_PM_VALUES) {
            cmdState = CMD_STATE_PM_VALUES;
          }
          break;
        default:
          break;
      }
      displayScreen();
      break;
    default:
      break;
  }
}

