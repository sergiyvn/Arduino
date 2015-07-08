#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

unsigned int data = 0;

volatile unsigned long int time1 = 0;
volatile unsigned long int time2 = 0;

void setup() {
  lcd.init();                     // инициализация LCD 
  lcd.backlight();                // включаем подсветку
  lcd.clear();                    // очистка дисплея
  
  Serial.begin(9600);

  attachInterrupt(0, sensor_1, FALLING);
  attachInterrupt(1, sensor_2, FALLING);
}

void loop()
{
    while ( time1 == 0 && time2 == 0 ) ;
    delay(800);

    if ( time1 != 0 && time2 != 0 && time2 > time1 )
    {
        data = 0.06 / ((time2 - time1) / 1000000.0); // v = s / t
    }
    else
    {
        data = 0;
    }

    Serial.println(data);
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print(data);


    time1 = 0;
    time2 = 0;
}

void sensor_1()
{
    if ( time1 == 0 )
    {
        time1 = micros();
    }
}

void sensor_2()
{
    if ( time2 == 0 )
    {
        time2 = micros();
    }
}
