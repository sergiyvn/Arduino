#include "LedControl.h"

/*
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn
 pin 11 is connected to the CLK
 pin 10 is connected to LOAD
 We have only a single MAX72XX.
 */

LedControl lc = LedControl(12, 11, 10, 1);

/* we always wait a bit between updates of the display */
unsigned long delaytime = 1000;

int i = 0;
int j = 9999;

void setup() {
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);
  // Display "- - - - - - -"
  //  lc.setRow(0, 0, B00000001);
  //  lc.setRow(0, 1, B00000001);
  //  lc.setRow(0, 2, B00000001);
  //  lc.setRow(0, 3, B00000001);
  //  lc.setRow(0, 4, B00000001);
  //  lc.setRow(0, 5, B00000001);
  //  lc.setRow(0, 6, B00000001);
  //  lc.setRow(0, 7, B00000001);

  delay(2000);
}

void loop() {
  printDigits(i, true);
  printDigits(j, false);
  i++;
  if (i > 9999)
    i = 0;
  j--;
  if (j < 0)
    j = 9999;
  delay(delaytime);
}

void printDigits(int value, boolean left)
{
  int padding = 0;
  if (left)
    padding = 4;
  int t = int(value / 1000);
  int s = int((value % 1000) / 100);
  int d = int(((value % 1000) % 100) / 10);
  int e = int(((value % 1000) % 100) % 10);
  if (value > 999)
    lc.setDigit(0, 3 + padding, t, false);
  else
    lc.setRow(0, 3 + padding, B00000000);//Очистка цифры
  if (value > 99)
    lc.setDigit(0, 2 + padding, s, false);
  else
    lc.setRow(0, 2 + padding, B00000000);//Очистка цифры
  if (value > 9)
    lc.setDigit(0, 1 + padding, d, false);
  else
    lc.setRow(0, 1 + padding, B00000000);//Очистка цифры
  lc.setDigit(0, 0 + padding, e, false);
}

