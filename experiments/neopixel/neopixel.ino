

#include <FastLED.h>


#define NUM_LEDS 1
#define DATA_PIN 8 // == D8
//#define DATA_PIN 5 // == D5
#define BRIGHTNESS 5

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting...\n");


  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  leds[0] = 0x999999;
  FastLED.show();
}

int pmsGetAqi() {
  return 2;
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

void ledAqi() {
  int aqival = pmsGetAqi();
  Serial.println(aqival);
  leds[0] = ledAqiValue(aqival);
  FastLED.show();
}

void loop() {
  delay(1000);
  ledAqi();

  //Serial.println(".");
  //  // Turn the LED on, then pause
  //  leds[0] = CRGB::Red;
  //  Serial.println("blink");
  //  FastLED.show();
  //  delay(500);
  //  // Now turn the LED off, then pause
  //  leds[0] = CRGB::Black;
  //  FastLED.show();
  //  delay(500);
}
