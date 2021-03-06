#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

unsigned int data = 0;
unsigned int prevData = 0;
unsigned int minValue = 1000;
unsigned int maxValue = 0;
unsigned int array[2];
unsigned int count = 0;

volatile unsigned long int time1 = 0;
volatile unsigned long int time2 = 0;

byte arrow_right[8] =
{
  B00000,
  B00000,
  B00010,
  B11111,
  B11111,
  B00010,
  B00000,
  B00000,
};

byte arrow_up[8] =
{
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
};

byte arrow_down[8] =
{
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
};



void setup() {
  lcd.init();                     // инициализация LCD
  lcd.backlight();                // включаем подсветку
  lcd.clear();                    // очистка дисплея
  lcd.createChar(0, arrow_right);
  lcd.createChar(1, arrow_up);
  lcd.createChar(2, arrow_down);


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
  count++;
  
  Serial.print("data: ");
  Serial.println(data);

  lcd.clear();
  lcd.setCursor(0, 0);
  //  lcd.print("Speed:");
  if (data != 0)
  {
    if (data < minValue)
      minValue = data;
    if (data > maxValue)
      maxValue = data;
  }

  lcd.setCursor(12, 0);
  lcd.write(byte(2));
  lcd.print(minValue);
  lcd.setCursor(12, 1);
  lcd.write(byte(1));
  lcd.print(maxValue);

  lcd.setCursor(0, 0);
  lcd.print(count);
  lcd.write(byte(0));
  if (data == 0)
    lcd.print("---");
  else
  {
    lcd.print(data);
    lcd.print(" m/s");
  }

  lcd.setCursor(0, 1);
  lcd.print("Last:");
  if (prevData == 0)
    lcd.print("---");
  else
  {
    lcd.print(prevData);
    lcd.print("");
  }

  prevData = data;
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
