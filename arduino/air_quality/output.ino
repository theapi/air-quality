
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
        displayScreen();
        mqttPublishReport();
        outputState = OUTPUT_STATE_READY;
        break;
      }
    default:
      break;
  }
}

void showText(char* text, int x, int y, int size, boolean d) {
  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(text);
  if (d) {
    display.display();
  }
}

void screenInit() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
  }

  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(1);
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text

  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setCursor(0, 0);     // Start at top-left corner

  char screen_buffer[20];
  sprintf(screen_buffer, "%x", ESP.getChipId());
  showText(screen_buffer, 0, 0, 2, true);

}

void displayScreen() {
  if (cmdState != CMD_STATE_TOGGLE_OFF) {
    screenAqi();
  }
}

void screenOff() {
  display.clearDisplay();
  display.display();
}

void screenAqi() {
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
}

