#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Bounce.h>

boolean hasNozzle = true;

int nozzlePin = 2;// Пин реле 1 (эл магнит)
int airPin = 3;// Пин реле 2 (эл клапан)
int btnPushPin = 4;// Пин курка
int btnUpPin = 5;// Пин up
int btnDownPin = 6;// Пин down
int btnNextPin = 7;// Пин next

int mode =  1;
int STEP =  5;

int nozleTime = 100;//ms
int nozleDelay = 100;//ms
int airTime = 100;//ms

Bounce bouncerPush = Bounce(btnPushPin, 40);
Bounce bouncerBtnUp = Bounce(btnUpPin, 40);
Bounce bouncerBtnDown = Bounce(btnDownPin, 40);
Bounce bouncerBtnNext = Bounce(btnNextPin, 40);

int buttonsDelay = 300; // ms

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                     // инициализация LCD
  lcd.backlight();                // включаем подсветку
  lcd.clear();                    // очистка дисплея
  pinMode(nozzlePin, OUTPUT); //Выход на реле нозла
  pinMode(airPin, OUTPUT); //Выход на реле клапана

  pinMode(btnPushPin, INPUT);   //переключаем Х вывод в режим входа
  digitalWrite(btnPushPin, 1);  //включаем на нем подтягивающий резистор

  pinMode(btnUpPin, INPUT);   //переключаем Х вывод в режим входа
  digitalWrite(btnUpPin, 1);  //включаем на нем подтягивающий резистор

  pinMode(btnDownPin, INPUT);   //переключаем Х вывод в режим входа
  digitalWrite(btnDownPin, 1);  //включаем на нем подтягивающий резистор

  pinMode(btnNextPin, INPUT);   //переключаем Х вывод в режим входа
  digitalWrite(btnNextPin, 1);  //включаем на нем подтягивающий резистор

  showNozzleTime();
}

void loop()
{
//  checkUpButton();
//  checkDownButton();
//  checkNextButton();
  checkPush();
}

void checkPush()
{
  if (bouncerPush.update())
  { //если произошло событие
    if (bouncerPush.read() == 0) {  //если кнопка нажата
      doPush();
      bouncerPush.rebounce(100);      //повторить событие через 500мс
    }
  }
}

void doPush()
{
  if (hasNozzle)
  {
    digitalWrite(nozzlePin, HIGH);
    delay(nozleTime);
    digitalWrite(nozzlePin, LOW);
    delay(nozleDelay);
  }
  
  digitalWrite(airPin, HIGH);
  delay(airTime);
  digitalWrite(airPin, LOW);
}

void showNozzleTime()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nozzle time:");
  lcd.setCursor(0, 13);
  lcd.print(nozleTime);
}

void showNozzleDelay()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nozzle del:");
  lcd.setCursor(0, 13);
  lcd.print(nozleTime);
}

void showAirTime()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Air del:");
  lcd.setCursor(0, 13);
  lcd.print(nozleTime);
}

void checkUpButton()
{
  if (bouncerBtnUp.update())
  { //если произошло событие
    if (bouncerBtnUp.read() == 0) {  //если кнопка нажата
      increase();
      printTimes();
      bouncerBtnUp.rebounce(buttonsDelay);      //повторить событие через 500мс
    }
  }
}


//Проверка для кнопки "Вниз" или нажата
void checkDownButton()
{
  if (bouncerBtnDown.update())
  { //если произошло событие
    if (bouncerBtnDown.read() == 0) {  //если кнопка нажата
      decrease();
      printTimes();
      bouncerBtnDown.rebounce(buttonsDelay);      //повторить событие через 500мс
    }
  }
}

//Проверка для кнопки "Next" или нажата
void checkNextButton()
{
  if (bouncerBtnNext.update())
  { //если произошло событие
    if (bouncerBtnNext.read() == 0) {  //если кнопка нажата
      mode += 1;
      if (mode > 3)
        mode = 1;
      printTimes();
      bouncerBtnNext.rebounce(buttonsDelay);      //повторить событие через 500мс
    }
  }
}

void increase()
{
  if (mode == 1)
    nozleTime += STEP;
  else if (mode == 2)
    nozleDelay += STEP;
  else if (mode == 3)
    airTime += STEP;
}

void decrease()
{
  if (mode == 1)
    nozleTime -= STEP;
  else if (mode == 2)
    nozleDelay -= STEP;
  else if (mode == 3)
    airTime -= STEP;
}

void printTimes()
{
  if (mode == 1)
    showNozzleTime();
  else if (mode == 2)
    showNozzleDelay();
  else if (mode == 3)
    showAirTime();
}
