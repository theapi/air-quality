

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
delay(1000);
}

void loop() {
  //Serial.println(".");
  // Turn the LED on, then pause
  leds[0] = CRGB::Red;
  Serial.println("blink");
  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
}
