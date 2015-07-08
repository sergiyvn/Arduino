// Demo with Font and icon Flash IC
// (C) 2013 Coldtears electronics
// For use with  CTE 3.2" (480x320) LCD Module w/Font and Icon Flash IC
// in combination with CTE TFT/SD Shield for Arduino Due
// web: http://stores.shop.ebay.com/coldtears-electronics-store
//
// The first part of the demo is to show how to use the font IC to draw font to the LCD,
// The IC also include some commonly used and open source icons
// great for custom application development.
//
// Second part of the demo code is adapted from Henning Karlsen's demo
// Original by
// UTFT_Demo_480x320 (C)2012 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This demo was made for modules with a screen resolution
// of 480x320 pixels.
// This sketch requires the UTFT library by Henning Karlsen
// also requires the Flash_IC library by Coldtears electronics
//


#include <UTFT.h>
#include <UTFT_CTE.h>

extern uint8_t SmallFont[];

// Set up UTFT for CTE32HR on Arduino Due
UTFT myGLCD(CTE32HR, 25, 26, 27, 28);
UTFT_CTE CTE_LCD(&myGLCD);

int clockCenterX = 240;
int clockCenterY = 160;

void setup()
{
  delay(1000);
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(VGA_BLACK);
  CTE_LCD.SPI_Flash_init(FLASH_CS_PIN);
}

void loop()
{
//  myGLCD.clrScr();
  // Draw crosshairs
//  myGLCD.setColor(0, 0, 255);
//  myGLCD.setBackColor(0, 0, 0);
  //  myGLCD.drawLine(239, 15, 239, 304);
  //  myGLCD.drawLine(1, 159, 478, 159);


  //  for (int i=0; i<20; i++)
  //  {
  //    myGLCD.drawCircle(clockCenterX, clockCenterY, 240-i);
  //  }

  //  drawMark(40);

  for (int i = 0; i < 180; i++)
    drawSector(i);

  delay(1000);

}

void drawMark(int h)//Отрисовка меток
{
  float x1, y1, x2, y2;

  h = h * 30;
  h = h;

  x1 = 100 * cos(h * 0.0175);
  y1 = 100 * sin(h * 0.0175);
  x2 = 110 * cos(h * 0.0175);
  y2 = 110 * sin(h * 0.0175);

  myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);
}



void drawSector(int s)
{
//  for (int i = 0; i < s; i++)
//  {
    float x1, y1, x2, y2;
//    int ps = i + 180;//start at the left point
    int ps = s + 180;//start at the left point

    myGLCD.setColor(0, 0, 0);

    x1 = 150 * cos(ps * 0.0175);
    x2 = 140 * cos(ps * 0.0175);
    y1 = 150 * sin(ps * 0.0175);
    y2 = 140 * sin(ps * 0.0175);

    myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);


//    myGLCD.setColor(255, 0, 0);
//  }

}


void drawSec(int s)//отрисовка секундной стрелки
{
  float x1, y1, x2, y2;
  int ps = s - 1;

  myGLCD.setColor(0, 0, 0);
  if (ps == -1)
    ps = 59;
  ps = ps * 6;
  ps = ps + 270;

  x1 = 95 * cos(ps * 0.0175);
  y1 = 95 * sin(ps * 0.0175);
  x2 = 5 * cos(ps * 0.0175);
  y2 = 5 * sin(ps * 0.0175);

  myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);

  myGLCD.setColor(255, 0, 0);
  s = s * 6;
  s = s + 270;

  x1 = 95 * cos(s * 0.0175);
  y1 = 95 * sin(s * 0.0175);
  x2 = 5 * cos(s * 0.0175);
  y2 = 5 * sin(s * 0.0175);

  myGLCD.drawLine(x1 + clockCenterX, y1 + clockCenterY, x2 + clockCenterX, y2 + clockCenterY);
}
