
enum CMDState {
  CMD_STATE_WAITING,
  CMD_STATE_TOGGLE_OFF,
};
CMDState cmdState = CMD_STATE_WAITING;

void handleCommand(char cmd, char param) {
  switch (cmd) {
    case 'D':
      switch (param) {
        case 't':
          Serial.println("display: toggle");
          if (cmdState != CMD_STATE_TOGGLE_OFF) {
            cmdState = CMD_STATE_TOGGLE_OFF;
            screenOff();
          } else {
            screenAqi();
            cmdState = CMD_STATE_WAITING;
          }
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

