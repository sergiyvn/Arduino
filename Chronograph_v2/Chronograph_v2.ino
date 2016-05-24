#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

unsigned int data = 0;

volatile unsigned long int time1 = 0;
volatile unsigned long int time2 = 0;

void setup() {
  lcd.init();                     // инициализация LCD
  lcd.backlight();                // включаем подсветку
  lcd.clear();                    // очистка дисплея

  Serial.begin(9600);

  lcd.setCursor(5, 0);
  lcd.print("Ready");
  lcd.setCursor(1, 1);
  lcd.print("Fire to start");

  attachInterrupt(1, sensor_1, FALLING);
  attachInterrupt(0, sensor_2, FALLING);
}

void loop()
{
  while ( time1 == 0 && time2 == 0 ) ;
  delay(800);

  if ( time1 != 0 && time2 != 0 && time2 > time1 )
  {
    data = 0.1 / ((time2 - time1) / 1000000.0); // v = s / t
  }
  else
  {
    data = 0;
  }

  Serial.print("data: ");
  Serial.println(data);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Speed:");

  lcd.setCursor(0, 1);
  if (data == 0)
    lcd.print("missed");
  else
  {
    lcd.print(data);
    lcd.print(" m/s");
  }



  time1 = 0;
  time2 = 0;
}

void sensor_1()
{
  if ( time1 == 0 )
  {
    time1 = micros();
//    Serial.print("time1: ");
//    Serial.println(time1);
  }
}

void sensor_2()
{
  if ( time2 == 0 )
  {
    time2 = micros();
//    Serial.print("time2: ");
//    Serial.println(time2);
  }
}
