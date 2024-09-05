



// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <U8g2_for_Adafruit_GFX.h>

GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS=D8*/ 5, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

uint16_t bg = GxEPD_WHITE;
uint16_t fg = GxEPD_BLACK;


byte fontSmallHeight = 17;
byte fontLargeHeight = 49;
byte lgsmSpace = 5;
byte readingSpace = 80;
int sensorHeight = 100;

unsigned long currentMillis = 0;
unsigned long sensorAtime = 0;
unsigned long sensorBtime = 0;
unsigned long sensorCtime = 0;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("starting");


  display.init();
  u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX
  display.setRotation(0);

  u8g2Fonts.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);            // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);         // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);         // apply Adafruit GFX color
  //u8g2Fonts.setFont(u8g2_font_logisoso28_tn);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall



  Serial.println(display.width());
  Serial.println(display.height());

  //helloWorld();

  Serial.println("setup done");

  currentMillis = millis();
  diplaySensorA(0, false);
  diplaySensorB(123, true);
  diplaySensorC(12, true);
}

void loop() {

}



void diplaySensorA(uint16_t aqi, bool current) {
  uint16_t x = 7;
  uint16_t y = fontLargeHeight + 5;
  byte xOffset = alignXOffset(aqi);

  if (!current) {
    aqi = 0;
  }

  display.firstPage();
  do
  {
    display.setPartialWindow(0, 0, display.width(), sensorHeight);
    display.fillScreen(bg);

    u8g2Fonts.setCursor(x + xOffset, y);
    u8g2Fonts.setFont(u8g2_font_fub49_tn);
    if (current) {
      sensorAtime = currentMillis;
      u8g2Fonts.print(aqi);
    } else {
      u8g2Fonts.setCursor(x + xOffset + 10, y);
      u8g2Fonts.print("-");
    }

    y = y + fontSmallHeight + lgsmSpace;
    u8g2Fonts.setCursor(x + 12, y);
    u8g2Fonts.setFont( u8g2_font_fur14_tr);
    u8g2Fonts.print("upstairs");

    u8g2Fonts.setCursor(x + display.width() - 43, y + 4);
    u8g2Fonts.setFont( u8g2_font_emoticons21_tr);
    if (current) {
      u8g2Fonts.print(emoticon(aqi));
    } else {
      u8g2Fonts.print("5");
    }

  }
  while (display.nextPage());
}

void diplaySensorB(uint16_t aqi, bool current) {
  uint16_t x = 7;
  uint16_t y = sensorHeight;
  byte xOffset = alignXOffset(aqi);

  if (!current) {
    aqi = 0;
  }

  display.firstPage();
  do
  {
    display.setPartialWindow(0, y, display.width(), sensorHeight);
    display.fillScreen(bg);

    y = y + fontLargeHeight + 5;
    u8g2Fonts.setCursor(x + xOffset, y);
    u8g2Fonts.setFont(u8g2_font_fub49_tn);
    if (current) {
      sensorBtime = currentMillis;
      u8g2Fonts.print(aqi);
    } else {
      u8g2Fonts.setCursor(x + xOffset + 10, y);
      u8g2Fonts.print("-");
    }

    y = y + fontSmallHeight + lgsmSpace;
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.setFont( u8g2_font_fur14_tr);
    u8g2Fonts.print("downstairs");

    u8g2Fonts.setCursor(x + display.width() - 30, y + 4);
    u8g2Fonts.setFont( u8g2_font_emoticons21_tr);
    if (current) {
      u8g2Fonts.print(emoticon(aqi));
    } else {
      u8g2Fonts.print("5");
    }

  }
  while (display.nextPage());
}

void diplaySensorC(uint16_t aqi, bool current) {
  uint16_t x = 7;
  uint16_t y = sensorHeight * 2;
  byte xOffset = alignXOffset(aqi);

  if (!current) {
    aqi = 0;
  }

  display.firstPage();
  do
  {
    display.setPartialWindow(0, y, display.width(), sensorHeight);
    display.fillScreen(bg);

    y = y + fontLargeHeight + 5;
    u8g2Fonts.setCursor(x + xOffset, y);
    u8g2Fonts.setFont(u8g2_font_fub49_tn);
    if (current) {
      sensorCtime = currentMillis;
      u8g2Fonts.print(aqi);
    } else {
      u8g2Fonts.setCursor(x + xOffset + 10, y);
      u8g2Fonts.print("-");
    }

    y = y + fontSmallHeight + lgsmSpace;
    u8g2Fonts.setCursor(x + 15, y);
    u8g2Fonts.setFont( u8g2_font_fur14_tr);
    u8g2Fonts.print("outside");

    u8g2Fonts.setCursor(x + display.width() - 45, y + 4);
    u8g2Fonts.setFont( u8g2_font_emoticons21_tr);
    if (current) {
      u8g2Fonts.print(emoticon(aqi));
    } else {
      u8g2Fonts.print("5");
    }

  }
  while (display.nextPage());
}

// https://raw.githubusercontent.com/wiki/olikraus/u8g2/fntpic/u8g2_font_emoticons21_tr.png
const char* emoticon(int aqival) {
  if (aqival <= 50) {
    return "6";
  }
  if (aqival <= 100) {
    return "#";
  }
  if (aqival <= 150) {
    return "&";
  }
  if (aqival <= 200) {
    return "'";
  }
  if (aqival <= 300) {
    return "(";
  }
  if (aqival > 300) {
    return ")";
  }
  return "5";
}

byte alignXOffset(int aqi) {
  if (aqi < 10) {
    return 35;
  }
  if (aqi < 100) {
    return 17;
  }
  return 0;
}

void helloWorld() {

  uint16_t x = 7;
  uint16_t y = fontLargeHeight + 5;

  uint16_t aqi = 0;
  byte xOffset = 0;

  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(bg);

    aqi = 82;
    xOffset = alignXOffset(aqi);
    u8g2Fonts.setCursor(x + xOffset, y);
    u8g2Fonts.setFont(u8g2_font_fub49_tn);
    u8g2Fonts.print(aqi);

    y = y + fontSmallHeight + lgsmSpace;
    u8g2Fonts.setCursor(x + 12, y);
    u8g2Fonts.setFont( u8g2_font_fur14_tr);
    u8g2Fonts.print("upstairs");

    u8g2Fonts.setCursor(x + display.width() - 43, y + 4);
    u8g2Fonts.setFont( u8g2_font_emoticons21_tr);
    u8g2Fonts.print(emoticon(aqi));

    aqi = 4;
    xOffset = alignXOffset(aqi);
    y = y + readingSpace;
    u8g2Fonts.setCursor(x + xOffset, y);
    u8g2Fonts.setFont(u8g2_font_fub49_tn);
    u8g2Fonts.print(aqi);

    y = y + fontSmallHeight + lgsmSpace;
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.setFont( u8g2_font_fur14_tr);
    u8g2Fonts.print("downstairs");

    u8g2Fonts.setCursor(x + display.width() - 30, y + 4);
    u8g2Fonts.setFont( u8g2_font_emoticons21_tr);
    u8g2Fonts.print(emoticon(aqi));

    aqi = 19;
    xOffset = alignXOffset(aqi);
    y = y + readingSpace;
    u8g2Fonts.setCursor(x + xOffset, y);
    u8g2Fonts.setFont(u8g2_font_fub49_tn);
    u8g2Fonts.print(aqi);

    y = y + fontSmallHeight + lgsmSpace;
    u8g2Fonts.setCursor(x + 15, y);
    u8g2Fonts.setFont( u8g2_font_fur14_tr);
    u8g2Fonts.print("outside");

    u8g2Fonts.setCursor(x + display.width() - 45, y + 4);
    u8g2Fonts.setFont( u8g2_font_emoticons21_tr);
    u8g2Fonts.print(emoticon(aqi));

  }
  while (display.nextPage());
}


