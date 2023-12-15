
#include <FastLED.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define NUM_LEDS 1
#define DATA_PIN 8 // == D8 on NodeMcu
#define BRIGHTNESS 126

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define GRAPH_WIDTH SCREEN_WIDTH - 1

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
CRGB leds[NUM_LEDS];

enum OutputState {
  OUTPUT_STATE_SETUP,
  OUTPUT_STATE_READY,
  OUTPUT_STATE_SENDING,
};
OutputState outputState = OUTPUT_STATE_SETUP;

unsigned long outputLastProcessed = 0;

int graph_buffer[GRAPH_WIDTH] = {0};

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
        graphUpdateData();
        mqttPublishReport();
        displayScreen();
        displayLed();
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

void ledInit() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  leds[0] = 0x999999;
  FastLED.show();
}

void ledSetBrightness(int val) {
  Serial.print("brightness: ");
  Serial.println(val);
  FastLED.setBrightness(val);
  FastLED.show();
}

void displayLed() {
  switch (ledState) {
    case LED_STATE_OFF:
      leds[0] = CRGB::Black;
      FastLED.show();
      break;
    case LED_STATE_ON:
      leds[0] = ledAqiValue(pmsGetAqi());
      FastLED.show();
      break;
  }
}

unsigned long ledAqiValue(int aqival) {
  if (aqival <= 50) {
    return CRGB::Green;
  }
  if (aqival <= 100) {
    return CRGB::Yellow;
  }
  if (aqival <= 150) {
    return CRGB::Orange;
  }
  if (aqival <= 200) {
    return CRGB::Red;
  }
  if (aqival <= 300) {
    return CRGB::Purple;
  }
  if (aqival > 300) {
    return CRGB::DarkViolet;
  }
  return CRGB::Blue;
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
  switch (displayState) {
    case DISPLAY_STATE_OFF:
      screenOff();
      break;
    case DISPLAY_STATE_AQI:
      screenAqi();
      break;
    case DISPLAY_STATE_GRAPH:
      screenGraph();
      break;
    case DISPLAY_STATE_PM_VALUES:
      screenPmValues();
      break;
    default:
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

void screenPmValues() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  char buf[50];
  sprintf(buf, " PM 1.0: %d   ", pmsGetPm10Env());
  display.println(buf);
  sprintf(buf, " PM 2.5: %d   ", pmsGetPm25Env());
  display.println(buf);
  sprintf(buf, "PM 10.0: %d   ", pmsGetPm100Env());
  display.println(buf);
  sprintf(buf, "    AQI: %d   ", pmsGetAqi());
  display.println(buf);
  display.display();
}

void graphUpdateData() {
  for (int k = 0; k < GRAPH_WIDTH; k++) {
    graph_buffer[k] = graph_buffer[k + 1];
  }
  graph_buffer[GRAPH_WIDTH - 1] = pmsGetAqi();
}

int maxGraphHeight() {
  int highest = 0;
  for (int x = 0; x < GRAPH_WIDTH; x++) {
    if (graph_buffer[x] > highest) {
      highest = graph_buffer[x];
    }
  }

  int m = 150;
  if (highest > m) {
    m = 300;
  }

  return m;
}

void screenGraph() {
  display.clearDisplay();
  char buf[20];
  int aqival = pmsGetAqi();
  sprintf(buf, "%d", aqival);

  showText(buf, 0, 0, 3, true);

  int max_height = maxGraphHeight();
  for (int16_t i = 0; i < GRAPH_WIDTH; i++) {
    int val = map(graph_buffer[i], 0, max_height, 0, SCREEN_HEIGHT);
    display.drawPixel(i, SCREEN_HEIGHT - (val + 1), SSD1306_WHITE);
  }

  display.display();
}


