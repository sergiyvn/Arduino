#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// For the breakout, you can use any (2 or) 3 pins
#define sclk 13//Mega only
#define mosi 11//Mega only
#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset

unsigned char i1, i2, c3;

//Adafruit_ST7735 display = Adafruit_ST7735(cs, dc, rst);// Uno
Adafruit_ST7735 display = Adafruit_ST7735(cs, dc, mosi, sclk, rst);// Mega

void setup() {
  display.initR(INITR_BLACKTAB);
  //  display.begin(SSD1306_SWITCHCAPVCC);
  display.fillScreen(ST7735_BLACK);
}

void loop() {
  TEST_display_1();
  delay(2000);
  TEST_display_2();
  delay(2000);
  TEST_display_3();
  delay(2000);
  TEST_display_4();
  delay(2000);
}

void TEST_display_1()
{
  display.fillScreen(ST7735_BLACK);
  //  display.clearDisplay();
  display.setTextColor(ST7735_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  for (i1 = 0; i1 < 8; i1++) {
    for (i2 = 0; i2 < 16; i2++) {
      c3 = i1 * 16 + i2;
      if (c3 == 0x0A || c3 == 0x0D) display.print(" ");
      else display.write(c3);
    }
    display.println("");
  }
//  display.display();
}

void TEST_display_2()
{
  display.fillScreen(ST7735_BLACK);
  //  display.clearDisplay();
  display.setTextColor(ST7735_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  for (i1 = 8; i1 < 16; i1++) {
    for (i2 = 0; i2 < 16; i2++)
      display.write(i1 * 16 + i2);
    display.println("");
  }
//  display.display();
}

void TEST_display_3()
{
  display.fillScreen(ST7735_BLACK);
  //  display.clearDisplay();
  display.setTextColor(ST7735_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.println(utf8rus("АБВГДЕЖЗИЙКЛМНОП"));
  display.println(utf8rus("РСТУФХЦЧШЩЪЫЬЭЮЯ"));
  display.println(utf8rus("абвгдежзийклмноп"));
  display.println(utf8rus("рстуфхцчшщъыьэюя"));
  display.println(utf8rus("Ёё123ABCabc!@#\xBC\xBD"));
  display.println(utf8rus("10\x83 10\x8A\x82 10\x81\x80 2\x85"));

//  display.display();
}

void TEST_display_4()
{
  display.fillScreen(ST7735_BLACK);
  //  display.clearDisplay();
  display.setTextColor(ST7735_WHITE);
  display.setCursor(0, 0);

  display.setTextSize(1);
  display.println(utf8rus("Размер шрифта 1"));

  display.setTextSize(2);
  display.println(utf8rus("Размер 2"));

  display.setTextSize(3);
  display.println(utf8rus("Разм 3"));

//  display.display();
}

