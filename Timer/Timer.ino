//
// Пин 8 Реле
//
// Пин 9 '+' пищалки
// GND   '-' пищалки
//
// *** Выходы для ЖК экрана ***
// GND экрана -> GND на плате
// VCC экрана -> +5В на плате
// SDA экрана -> A4 на плате (Analog IN)
// SCL экрана -> A5  на плате (Analog IN)

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Bounce.h>
#include <Arduino.h>
#include <Keypad.h>
#include <Eeprom24C01_02.h>

#define EEPROM_ADDRESS  0x50

String deviceVersion = "1.0.1"; // Версия прибора :)

static Eeprom24C01_02 eeprom(EEPROM_ADDRESS);
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const byte numRows = 4; //number of rows on the keypad
const byte numCols = 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char ints[][2] = {
  {'1', 1},
  {'2', 2},
  {'3', 3},
  {'4', 4},
  {'5', 5},
  {'6', 6},
  {'7', 7},
  {'8', 8},
  {'9', 9},
  {'0', 0}
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {7, 6, 5, 4}; //Rows 0 to 3
byte colPins[numCols] = {3, 2, 1, 0}; //Columns 0 to 3

//initializes an instance of the Keypad class
Keypad keypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

int relePin = 8;// Пин реле
int beepPin = 9; //выход пищалки
int vibroPin = 10;//Пин датчика вибрации

Bounce bouncerVibro = Bounce(vibroPin, 40); //создаем экземпляр класса Bounce для 10 вывода

int GAME_MODE = 1;// Режим игру по умолчанию
int GAME_COUNTS = 1;// количество режимов игры

char* MODE_NAME[] = {
  "  - TAUMEP ",
  "  - HETY        ",
  "  - HETY        ",
  "  - HETY        ",
  "  - HETY        "
};

boolean is_mode_selected = false;//Проверяет или выбран режим игры
boolean is_mode_setup = false; // Проверяет установлены ли параметры выбраного режим игры

/** Опции игры */
boolean option_vibro = false; // Датчик вибрации включен?
boolean option_vibro_is_setup = false;

int option_vibro_delay_time = 60; // время в секундах, Датчик вибрации включен?
boolean option_vibro_delay_time_is_setup = false;

boolean option_detonator = false; // Реле для петарды включено?
boolean option_detonator_is_setup = false;

int buttonsDelay = 300; // ms
boolean gameStarted = false; //Press reset button to start game mode

int CODE1 = -1;
int CODE2 = -1;
int CODE3 = -1;
int CODE4 = -1;
int _CODE = 0;//текущий код
int codeInputted = 0;
int _codeInputted = 0;
boolean is_code_setted = false; //

long TIME = 5000; //5sec Timer time
long _TIME = 5; //временный параметр Timer time
int displayTimeInt = 0;
boolean is_time_setup = false;

//private vars
float divider = 1000;// 1000 для мс, 1000000 для микросекунд
float starTime;
boolean stopped = false;
boolean timeEnd = false;
boolean correctCode = false;

long starBlinkTime;// время начала переключения диода (мс)
long blinkDelay = 1000.0;//1sec время моргания диодов
long currentTime;// текущие время (мс)

void setup() {
  // Initialize EEPROM library.
  eeprom.initialize();

  // Write a byte at address 0 in EEPROM memory.
  eeprom.writeByte(1, 123);
  //Делаем обязательную задержку
  delay(10);
  // Read a byte at address 0 in EEPROM memory.
  byte data = eeprom.readByte(1);
  lcd.print(data);

  lcd.init();                     // инициализация LCD
  lcd.backlight();                // включаем подсветку
  lcd.clear();                    // очистка дисплея

  pinMode(relePin, OUTPUT); //Выход на реле
  pinMode(beepPin, OUTPUT); //Выход на пищалку

  //Serial.begin(9600);
  //Serial.print(" * * START * * ");
  //Serial.print("\n");
  lcd.setCursor(0, 0);
  //  lcd.print(" A-Team Timer");
  //  lcd.setCursor(0, 1);
  //  lcd.print(" version ");
  //  lcd.print(deviceVersion);

  //  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setup mode:");
  lcd.setCursor(13, 0);
  lcd.print(GAME_MODE);
  lcd.setCursor(0, 1);
  lcd.print(MODE_NAME[0]);
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();

  if (key != NO_KEY) {
    //key
    beep(50);

    //Проверям сначало установку режима
    if (!is_mode_selected)
    {
      mode_Function(key);
    }
    // включение датчика вибрации
    else if (!option_vibro_is_setup)
    {
      vibro_Function(key);
    }
    //установка время на выход из строя прибора
    else if (!option_vibro_delay_time_is_setup)
    {
      vibroTime_Function(key);
    }
    //Установка детонатора
    else if (!option_detonator_is_setup)
    {
      detonator_Function(key);
    }
    //Устанавливаем код
    else if (!is_code_setted)
    {
      code_Function(key);
    }
    //Устанавливаем время таймера
    else if (!is_time_setup)
    {
      time_Function(key);
    }
    //Ждем запуска игры
    else if (!gameStarted)
    {
      checkStartButton(key);
    }
  }

  // Запускаем сценарий игры
  if (is_mode_selected && is_code_setted && is_mode_setup && gameStarted && !stopped)
  {
    switch (GAME_MODE)
    {
      // **********/ GAME MODE 1 BEGIN /*********************************************
      case 1:
        runMode_1(key);
        break;
      // **********/ GAME MODE 2 BEGIN /*********************************************
      case 2:
        runMode_2(key);
        break;
      // **********/ GAME MODE 3 BEGIN /*********************************************
      case 3:
        runMode_3(key);
        break;
      // **********/ GAME MODE 4 BEGIN /*********************************************
      case 4:
        runMode_4(key);
        break;
      // **********/ GAME MODE 5 BEGIN /*********************************************
      case 5:
        runMode_5(key);
        break;
    }
  }
}

void mode_Function(char key)
{
  if (key == 'A') {
    GAME_MODE ++;
    if (GAME_MODE > GAME_COUNTS)
      GAME_MODE = 1;
  } else if (key == 'D')
  {
    GAME_MODE --;
    if (GAME_MODE < 1)
      GAME_MODE = GAME_COUNTS;
  }
  else if (key == '#')
  {
    // Устанавливаем параметры выбраного режима игры
    is_mode_selected = true;
    setupMode();
  }

  if (key == 'A' || key == 'D')
  {
    lcd.setCursor(13, 0);
    lcd.print(GAME_MODE);
    lcd.setCursor(0, 1);
    lcd.print(MODE_NAME[GAME_MODE - 1]);
  }
}



//Установка параметров для определенного режима игры
void setupMode()
{
  //Установка режимов
  switch (GAME_MODE)
  {
    case 1:
      //      option_vibro_delay_time = 60; //Вывод из строя прибора на 5мин по умолчанию
      TIME = 60000;
      _TIME = 60;
      //      RANGE_TIME = 100;
      //      RANGE_TIME_BEGIN = 0;//мс //пока не использую
      //      RANGE_TIME_END = 100;//мс //пока не использую
      //      can_disconnect = true;
      //      is_disconnect_setup = true;
      //      canRestart = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vibration on?:");
      lcd.setCursor(10, 1);
      lcd.print(" No");
      //      starTime = millis();
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
  }

  is_mode_setup = true;
}

void vibro_Function(char key)
{
  if (key == 'A' || key == 'D')
  {
    option_vibro = !option_vibro;
    lcd.setCursor(10, 1);
    if (option_vibro)
      lcd.print("Yes");
    else
      lcd.print(" No");
  }

  if (key == '#')
  {
    if (option_vibro)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Deactivation");
      lcd.setCursor(4, 1);
      lcd.print("time:");
      lcd.setCursor(9, 1);
      lcd.print(option_vibro_delay_time);
      lcd.setCursor(12, 1);
      lcd.print("sec");
    } else
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Detonator on?:");
      lcd.setCursor(10, 1);
      lcd.print(" No");
      option_vibro_delay_time_is_setup = true;
    }

    option_vibro_is_setup = true;
  }
}

void vibroTime_Function(char key)
{
  if (key == 'A')
  {
    option_vibro_delay_time += 5;
  }
  else if (key == 'D')
  {
    option_vibro_delay_time -= 5;
    if (option_vibro_delay_time < 0)
      option_vibro_delay_time = 0;
  }

  if (key == 'A' || key == 'D')
  {
    lcd.setCursor(9, 1);
    lcd.print("   ");
    lcd.setCursor(9, 1);
    lcd.print(option_vibro_delay_time);
    lcd.setCursor(12, 1);
    lcd.print("sec");
  }

  if (key == '#')
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Detonator on?:");
    lcd.setCursor(10, 1);
    lcd.print(" No");

    if (option_vibro_delay_time <= 0)
      option_vibro = false;

    option_vibro_delay_time_is_setup = true;
  }
}


void detonator_Function(char key)
{
  if (key == 'A' || key == 'D')
  {
    option_detonator = !option_detonator;
    lcd.setCursor(10, 1);
    if (option_detonator)
      lcd.print("Yes");
    else
      lcd.print(" No");
  }

  if (key == '#')
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set code: ");
    lcd.setCursor(10, 0);
    lcd.print("_");

    option_detonator_is_setup = true;
  }
}

void code_Function(char key)
{
  if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4'
      || key == '5' || key == '6' || key == '7' || key == '8' || key == '9')
  {
    _CODE = FindInt(key);

    if (CODE1 == -1)
    {
      lcd.setCursor(10, 0);
      CODE1 = _CODE;
    }
    else if (CODE2 == -1)
    {
      lcd.setCursor(11, 0);
      CODE2 = _CODE;
    }
    else if (CODE3 == -1)
    {
      lcd.setCursor(12, 0);
      CODE3 = _CODE;
    }
    else if (CODE4 == -1)
    {
      lcd.setCursor(13, 0);
      CODE4 = _CODE;
      lcd.print(_CODE);
    }
    if (CODE4 == -1)
    {
      lcd.print(_CODE);
      lcd.print("_");
    }
  }
  else if (key == '#' && CODE4 != -1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Setup timer");
    lcd.setCursor(0, 1);
    lcd.print("time:");
    lcd.setCursor(6, 1);
    //    float _t = TIME / 1000;
    //    lcd.print(int(_t));
    //    lcd.print(" sec");
    lcd.print(getTimeString(int(_TIME)));

    is_code_setted = true;
  }
  else if (key == '*')
  {
    CODE1 = -1;
    CODE2 = -1;
    CODE3 = -1;
    CODE4 = -1;
    _CODE = 0;//текущий код

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set code: ");
    lcd.setCursor(10, 0);
    lcd.print("_");
  }
}


void time_Function(char key)
{
  if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4'
      || key == '5' || key == '6' || key == '7' || key == '8' || key == '9')
  {
    int _t = FindInt(key);
    setTime(_t);
    if (_TIME > 5959 || _TIME < 0)
      _TIME = 5959;


    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Setup timer");
    lcd.setCursor(0, 1);
    lcd.print("time:");
    lcd.setCursor(6, 1);
    lcd.print(getTimeString(int(_TIME)));
  }
  else if (key == '#')
  {
    TIME = _TIME * divider * 60;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Press button");
    lcd.setCursor(0, 1);
    lcd.print("  'A' to START");

    is_time_setup = true;
  }
  else if (key == '*')
  {
    _TIME = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Setup timer");
    lcd.setCursor(0, 1);
    lcd.print("time:");
    lcd.setCursor(6, 1);
    lcd.print(getTimeString(int(_TIME)));
  }
}

//Ждем нажатия кнопки до запуска игры
void checkStartButton(char key)
{
  if (key == 'A')
  {
    gameStarted = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    printTime(TIME, 0, 0);
    lcd.setCursor(0, 1);
    lcd.print("Code: ");
    delay(1000);
    starTime = millis();
    starBlinkTime = millis();
  }
}


//  Запуск  режима 1
void runMode_1(char key)
{
  if (!stopped)
  {
    float _currentTime = millis();
    float tm = starTime + TIME - _currentTime;
    //Время вышло взрываем бомбу
    if (tm <= 0)
    {
      stopped = true;
      timeEnd = true;
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print(" Time end");
      lcd.setCursor(0, 1);
      lcd.print(" Bomb detonated");

      //Запуск детонации бомбы /включаем питание на реле на 2сек и выключаем
      detonateBomb();

      for (int i = 0; i < 100; i++)
        beep(50);
    }
    else if (codeInputted == 0)
    {
      printTime(tm, 0, 0);

      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4'
          || key == '5' || key == '6' || key == '7' || key == '8' || key == '9')
      {
        int _t = FindInt(key);
        if (_codeInputted * 10 + _t < 9999)
          _codeInputted = _codeInputted * 10 + _t;

        if (_codeInputted > 9999 || _codeInputted < 0)
          _codeInputted = 0;

        lcd.setCursor(0, 1);
        lcd.print("Code:");
        lcd.setCursor(6, 1);
        lcd.print(int(_codeInputted));
      }
      else if (key == '#')
      {
        if (_codeInputted > 999)
          codeInputted = _codeInputted;
      }
      else if (key == '*')
      {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Code:");
        codeInputted = 0;
        _codeInputted = 0;
      }
    }
    //Проверяем введенный код
    else if (codeInputted != 0 )
    {
      int _realCode = CODE1 * 1000 + CODE2 * 100 + CODE3 * 10 + CODE4;
      if (_realCode == codeInputted)
        correctCode = true;
      //    if (!correctCode && !timeEnd ) {
      //      beep(50);
      //    }

      if (correctCode)
      {
        stopped = true;
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Congratulation");
        lcd.setCursor(0, 1);
        lcd.print("Bomb deactivated");
      } else {
        printTime(tm, 0, 0);
        lcd.setCursor(0, 1);
        lcd.print("Wrong, Try again");
        if (key == '*')
        {
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("Code:");
          codeInputted = 0;
          _codeInputted = 0;
        }
      }
    }

    //Gищание пищалки раз в секунд
    currentTime = millis();
    if ( starBlinkTime + blinkDelay < currentTime)
    {
      starBlinkTime = millis();
      beep(50);
    }

  }
}



//  Запуск  режима 2
void runMode_2(char key)
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Wrong mode 2");
}

//  Запуск  режима 3
void runMode_3(char key)
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Wrong mode 3");
}

//  Запуск  режима 4
void runMode_4(char key)
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Wrong mode 4");
}

//  Запуск  режима 5
void runMode_5(char key)
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Wrong mode 5");
}


void checkVibro()
{
  if (option_vibro)
    if (bouncerVibro.update())
      //если произошло событие
      if (bouncerVibro.read() == 0) {  //если кнопка нажата
        bouncerVibro.rebounce(option_vibro_delay_time * divider);    //повторить событие через 1000мс

        //Выводим сообщение о выходе из строя прибора
        lcd.clear();
        lcd.setCursor(0, 0);
        //        lcd.print("Device damaged");
        //        lcd.setCursor(0, 1);
        //        lcd.print("Reload time:");
        //        lcd.print(option_vibro_delay_time);
        //        lcd.print("s");

        lcd.print(" Bomb detonated");

        //Запуск детонации бомбы /включаем питание на реле на 2сек и выключаем
        detonateBomb();

        if (option_vibro_delay_time - 2 > 0)
        {
          delay((option_vibro_delay_time - 2 ) * divider); //Задержка сценария на заданое время
        }
        //          if (canRestart)
        //            resetMode();

        bouncerVibro = Bounce(vibroPin, 40);
      }
}

void detonateBomb()
{
  //Запуск детонации бомбы /включаем питание на реле на 2сек и выключаем
  if (option_detonator)
  {
    digitalWrite(relePin, HIGH);
    delay(2000);
    digitalWrite(relePin, LOW);
    option_detonator = false;
  }
}


void beep(unsigned char delayms) {
  //  Для пьезопещалки
  //  analogWrite(beepPin, 20);      // значение должно находится между 0 и 255
  //  delay(delayms);          // пауза delayms мс
  //  analogWrite(beepPin, 0);       // 0 - выключаем пьезо
  //  delay(delayms);          // пауза delayms мс

  //Для динамиков
  tone(beepPin, 950, delayms);
  delay(2 * delayms);
}

//Конвертирует символ '1' в цыфру 1
int FindInt(char keyCode) {
  for (int i = 0; i < 16; i++) {
    if (ints[i][0] == keyCode) {
      return ints[i][1];
    }
  }
  return -1; //если код на найден возвращаем 100
}


void printTime(float value, int x, int y)
{
  float s = value / divider;

  lcd.setCursor(x, y);
  lcd.print("Time : ");
  lcd.print(s);
}


void setTime(int t)
{
  float _t = _TIME * 10 + t;
  _TIME = _t;
}


String getTimeString(int value)
{
  if (value == 0)
  {
    _TIME = 0;
    return "00:00";
  }

  String displayTimeStr;
  int h = 0;
  int m = 0;

  h = int(value / 100);
  m = (value % 100);

  if (h > 59)
    h = 59;
  if (m > 59)
    m = 59;

  int tt = h * 3600 + m * 60;
  _TIME = h * 100 + m;
  return getFormattedTime(tt, false);
}

String getFormattedTime(int value, boolean hasSeconds)
{
  String formattedTime;

  if (value / 3600 < 10)
    formattedTime += '0';
  formattedTime += value / 3600;
  formattedTime += ':';

  if ( (value % 3600) / 60 < 10)
    formattedTime += '0';
  formattedTime += (value % 3600) / 60;

  if (!hasSeconds)
    return formattedTime;

  formattedTime += ':';
  if ((value % 3600) % 60 < 10)
    formattedTime += '0';
  formattedTime += (value % 3600) % 60;
  return formattedTime;
}
