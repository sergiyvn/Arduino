#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

int i = 0;

void setup() {
  lcd.init();                     // инициализация LCD
  lcd.backlight();                // включаем подсветку
  lcd.clear();                    // очистка дисплея
}
//132 133  223 градус
void loop()
{
  lcd.setCursor(0, 0);
  lcd.print(i);
  lcd.setCursor(4, 0);
  lcd.write(byte(187));
//  i++;
  delay(500);
}

