// LCD5110_Graph_Demo
//
//      VCS - 3.3V
//      SCK  - Pin 8
//      MOSI - Pin 9
//      DC   - Pin 10
//      RST  - Pin 11
//      CS   - Pin 12
//
#include <LCD5110_Graph.h>

LCD5110 myGLCD(8, 9, 10, 12, 11);

extern unsigned char TinyFont[];
extern uint8_t SmallFont[];

//Пин подключен к ST_CP входу 74HC595
int latchPin = 8;
//Пин подключен к SH_CP входу 74HC595
int clockPin = 12;
//Пин подключен к DS входу 74HC595
int dataPin = 11;


void setup()
{
  //устанавливаем режим OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  randomSeed(analogRead(7));
}

void loop()
{
     digitalWrite(latchPin, LOW);
    // передаем последовательно на dataPin
    shiftOut(dataPin, clockPin, MSBFIRST, 111); 
 
    //"защелкиваем" регистр, тем самым устанавливая значения на выходах
    digitalWrite(latchPin, HIGH);
    // пауза перед следующей итерацией
    delay(500);
  
  myGLCD.clrScr();
  myGLCD.update();
  
  //  myGLCD.clrScr();
  //  myGLCD.drawBitmap(0, 0, arduino_logo, 84, 48);
  //  myGLCD.update();
  //
  myGLCD.drawRect(28, 18, 56, 28);
  for (int i = 0; i < 6; i++)
  {
    myGLCD.drawLine(57, 18 + (i * 2), 83 - (i * 3), 18 + (i * 2));
  }
 myGLCD.update();
  delay(2000);

  myGLCD.clrScr();
  myGLCD.update();
  myGLCD.print("LCD5110_Graph", CENTER, 0);
  myGLCD.print("LCD5110_Graph", CENTER, 5);
  myGLCD.print("LCD5110_Graph", CENTER, 10);
  myGLCD.print("LCD5110_Graph", CENTER, 15);
  myGLCD.print("DEMO", CENTER, 20);
  myGLCD.print("DEMO", CENTER, 25);
  myGLCD.print("DEMO", CENTER, 30);
  myGLCD.print("DEMO", CENTER, 35);
  myGLCD.print("DEMO", CENTER, 40);
  myGLCD.print("DEMO", CENTER, 45);
  myGLCD.print("DEMO", CENTER, 50);
  myGLCD.update();
  
  delay(2000);
}
