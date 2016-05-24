/***************************************************
  This is an example sketch for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 ****************************************************/

// For the breakout, you can use any (2 or) 3 pins
//#define sclk 13
//#define mosi 11
#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

float p = 3.1415926;

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 3

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

DeviceAddress roomThermometer = {0x28, 0xFF, 0x53, 0x2F, 0x93, 0x15, 0x03, 0x44 };
DeviceAddress kitchenThermometer = {0x28, 0xFF, 0x1C, 0x2E, 0x93, 0x15, 0x03, 0x4D };
//28 FF 53 2F 93 15 3 44
//28 FF 1C 2E 93 15 3 4D
int x = 0;
unsigned long currentTime = 0;//milliseconds
unsigned long startTime = 0;//milliseconds
unsigned long time = 60000; //5mins
float tempC = 0;
float tempCprev = 0;

void setup(void) {
  Serial.begin(9600);
  sensors.begin();
  sensors.setResolution(roomThermometer, 10);
  //  Serial.print("hello!");

  // If your TFT's plastic wrap has a Black Tab, use the following:
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // If your TFT's plastic wrap has a Red Tab, use the following:
  //tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  //tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab

  tft.fillScreen(ST7735_BLACK);

  //  // line draw test
  //  testlines(ST7735_YELLOW);
  //  delay(500);
  //
  //  // optimized lines
  //  testfastlines(ST7735_RED, ST7735_BLUE);
  //  delay(500);
  startTime = millis();
}



void loop() {
  currentTime = millis();
  unsigned long _t = currentTime - startTime;


  sensors.requestTemperatures();
  float tempRoom = sensors.getTempC(roomThermometer);
  float tempCKitchen = sensors.getTempC(kitchenThermometer);

  //    if (tempCprev != tempC)
  //    {
  //      tft.fillRect(0, 0 , tft.width(), 20, ST7735_BLACK);
  //      tft.setCursor(10, 5);
  //      tft.setTextColor(ST7735_RED);
  //      tft.setTextSize(2);
  //
  //      if (tempC == -127.00) {
  //        tft.println("Error getting temperature");
  //      } else {
  //        tft.print("C: ");
  //        tft.print(tempC);
  //      }
  //    }

  tft.fillRect(0, 0 , tft.width(), 20, ST7735_BLACK);
  drawText(tempRoom, ST7735_RED, 5);
  drawText(tempCKitchen, ST7735_RED, 69);

  //    if (tempC != -127.00)
  if (_t >= time)
  {
    tft.drawPixel(x, 160 - tempRoom * 3, ST7735_GREEN);
    tft.drawPixel(x, 160 - tempCKitchen * 3, ST7735_BLUE);

    //    delay(1000 * time);
    //    tempCprev = tempC;
    x++;
    if (x > 128)
    {
      x = 0;
      tft.fillScreen(ST7735_BLACK);
    }
    startTime = millis();
  }

  delay(1000);
}

void drawText(float tempC, uint16_t color, int x)
{
  tft.setCursor(x, 5);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  //      tft.print("C: ");
  tft.print(tempC);
}

void testlines(uint16_t color) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, 0, x, tft.height() - 1, color);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, 0, tft.width() - 1, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, color);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, 0, 0, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, tft.height() - 1, x, 0, color);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, color);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, color);
  }
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t y = 0; y < tft.height(); y += 5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x = 0; x < tft.width(); x += 5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}


