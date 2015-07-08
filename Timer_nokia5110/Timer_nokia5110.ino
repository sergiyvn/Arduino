// Режимы:
// 1. UnrealTournament таймер
// 2. Вывод кода на время ХХ через время YY

#include <Wire.h>
#include <Bounce.h>
#include <Eeprom24C01_02.h>
#include <LCD5110_Graph.h>

#define EEPROM_ADDRESS  0x50

//
//      VCS - 3.3V
//      SCK  - Pin 8
//      MOSI - Pin 9
//      DC   - Pin 10
//      RST  - Pin 11
//      CS   - Pin 12
//

#include <LCD5110_Graph.h>

//static Eeprom24C01_02 eeprom(EEPROM_ADDRESS);

LCD5110 myGLCD(7, 10, 11, 13, 12);

extern unsigned char TinyFont[];
extern uint8_t SmallFont[];

int ledPin1 = 0;// диод команда 1
int ledPin2 = 1;// диод команда 2
int ledPinN = 2;// диод нейтралы
int but1Pin = 3;// кнопка команда 1
int but2Pin = 4;// кнопка команда 2
int butNPin = 5;// кнопка нейтралы
int beepPin = 9;// выход на динамик

Bounce bouncerButton1 = Bounce(but1Pin, 40); //создаем экземпляр класса Bounce для 3 вывода
Bounce bouncerButton2 = Bounce(but2Pin, 40); //создаем экземпляр класса Bounce для 4 вывода
Bounce bouncerButtonN = Bounce(butNPin, 40); //создаем экземпляр класса Bounce для 5 вывода

int GAME_MODE = 1;// Режим игру по умолчанию
int GAME_COUNTS = 2;// количество режимов игры

boolean is_mode_selected = false;//Проверяет или выбран режим игры
boolean is_mode_setup = false; // Проверяет установлены ли параметры выбраного режим игры

long starTime;// время начала игры (мс)
long currentTime;// текущие время игры (мс)
long summaryTimeTeam1 = 0;// сумарное время команды 1 (мс)
long summaryTimeTeam2 = 0;// сумарное время команды 2 (мс)
long _prevTimeDelay = 1000;//
long _prevSummaryTimeTeam1 = 1000;//
long _prevSummaryTimeTeam2 = 1000;//

long timeBegin = 0;// начало время команды
long _summaryTime = 0;//время удержания

long TIME = 5.0 * 60.0 * 1000.0; //5mins время игры в мс
long _TIME = 5; //временный параметр для игры в минутах
long TIME_DELAY_BEFORE_GAME = 10.0 * 1000.0;//5 * 60 * 1000; //5mins время до старта в мс
long _TIME_DELAY_BEFORE_GAME = 10; //временный параметр для старта в минутах
long TIME_SHOW_CODE = 10.0 * 1000.0;// время отображения кода в мс
long _TIME_SHOW_CODE = 10; // время отображения кода в минутах

long _CODE_NUMBER = 0;// Текущея цифра кода
long CODE = 0;// КОД
long CODE_LENGTH = 4;// Количество цифр в коде
int DIGIT1 = -1;// КОД1
int DIGIT2 = -1;// КОД2
int DIGIT3 = -1;// КОД3
int DIGIT4 = -1;// КОД4
int DIGIT5 = -1;// КОД5
int DIGIT6 = -1;// КОД6
int DIGIT7 = -1;// КОД7
int DIGIT8 = -1;// КОД8


boolean isSetupDelay = false;// Установлено время до начала игры?
boolean isSetupGameTime = false;// Установлено время игры?
boolean isGameDelayStarted = false;// Запуcк задержки игры по кнопке ОК
boolean isShowCodeTimeSetup = false;// Задало ли время отображения кода?
boolean isGameStarted = false;// Запушена игра время игры?
boolean isCodeLength = false;// Запушена игра время игры?
boolean isCodeSetup = false;// Запушена игра время игры?

boolean holdingTeam1 = false;
boolean holdingTeam2 = false;

long divider = 1000.0;// 1000 для мс, 1000000 для микросекунд
long dividerMins = 60.0 * 1000.0;// 60 * 1000 для мс, 60 * 1000000 для микросекунд
long showMins = 60.0;// Делить результа ты на 60сек для отображения только минут

long starBlinkTime;// время начала переключения диода (мс)
long blinkDelay = 1000.0;//1sec время моргания диодов
int diodeBlink = false;
boolean showingCode = false;

float DELAY_STEP = 5.0;// mins
float TIME_STEP = 10.0;/// mins
float SHOW_TIME_STEP = 5.0;// sec

int buttonsDelay = 300; // задержка между нажатиями кнопки ms


void setup() {
  /*
    // Initialize EEPROM library.
    eeprom.initialize();

    // Write a byte at address 0 in EEPROM memory.
    eeprom.writeByte(1, 123);
    //Делаем обязательную задержку
    delay(10);
    // Read a byte at address 0 in EEPROM memory.
    byte data = eeprom.readByte(1);
  */

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPinN, OUTPUT);
  pinMode(beepPin, OUTPUT);

  delay(1000);

  // Пищим три раза при старте прибора :)
  beep(50);
  beep(50);
  beep(50);

  myGLCD.InitLCD();
  //  myGLCD.setFont(SmallFont);
  myGLCD.setFont(TinyFont);

  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  digitalWrite(ledPinN, HIGH);

  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPinN, LOW);

  // Выводим режим игры
  myGLCD.print("Выберите режим", CENTER, 10);
  myGLCD.print(intToChar(GAME_MODE), CENTER, 30);
  myGLCD.update();

  //  //Serial.begin(9600);
  //  //Serial.print(" * * START * * ");
  //  //Serial.print("\n");

}

void loop() {
  currentTime =  millis();

  if (is_mode_selected && !is_mode_setup)
  {
    setupModeStandardConfig();
  }
  else if (isSetupDelay && isSetupGameTime && isGameDelayStarted)
  {
    if (isGameStarted)
    {
      switch (GAME_MODE)
      {
        // **********/ GAME MODE 1 BEGIN /*********************************************
        case 1:
          runMode_1();
          break;
        // **********/ GAME MODE 2 BEGIN /*********************************************
        case 2:
          runMode_2();
          break;
        // **********/ GAME MODE 3 BEGIN /*********************************************
        case 3:
          runMode_3();
          break;
        // **********/ GAME MODE 4 BEGIN /*********************************************
        case 4:
          runMode_4();
          break;
        // **********/ GAME MODE 5 BEGIN /*********************************************
        case 5:
          runMode_5();
          break;
      }
    } else {
      if (starTime + TIME_DELAY_BEFORE_GAME < currentTime)
      {
        starTime = millis();
        //        isGameStarted = true;
        if (GAME_MODE == 1)
        {
          if (!isGameStarted)
          {
            if (summaryTimeTeam1 / 1000 != _prevSummaryTimeTeam1)
            {
              myGLCD.clrScr();
              myGLCD.print("Team 1: ", CENTER, 0);
              myGLCD.print(longintToChar(summaryTimeTeam1 / 1000), CENTER, 10);
              myGLCD.print("Team 2", CENTER, 24);
              myGLCD.print(longintToChar(summaryTimeTeam2 / 1000), CENTER, 34);
              myGLCD.update();

              _prevSummaryTimeTeam1 = summaryTimeTeam1 / 1000;
            }
            if (summaryTimeTeam2 / 1000 != _prevSummaryTimeTeam2)
            {
              myGLCD.clrScr();
              myGLCD.print("Team 1: ", CENTER, 0);
              myGLCD.print(longintToChar(summaryTimeTeam1 / 1000), CENTER, 10);
              myGLCD.print("Team 2", CENTER, 24);
              myGLCD.print(longintToChar(summaryTimeTeam2 / 1000), CENTER, 34);
              myGLCD.update();

              _prevSummaryTimeTeam2 = summaryTimeTeam2 / 1000;
            }
            //Serial.print(" Time1:  ");
            //Serial.print(summaryTimeTeam1 / 1000);
            //Serial.print("     Time2:  ");
            //Serial.print(summaryTimeTeam2 / 1000);
            //Serial.print("\n");
          }
        } else {
          //Serial.print(" ********* clearDisplay ********* ");
          myGLCD.clrScr();
          myGLCD.update();
        }

        isGameStarted = true;
      } else {
        //Выводим время до старта основной игры
        long et = (starTime + TIME_DELAY_BEFORE_GAME - currentTime) / 1000;
        if (int(_prevTimeDelay) != int(et)) {
          myGLCD.clrScr();
          myGLCD.print("Игра начнеться через", CENTER, 10);
          myGLCD.print(longintToChar(et), CENTER, 30);
          myGLCD.update();

          _prevTimeDelay = et;
          //Serial.print(" Time to game:  ");
          //Serial.print(et);
          //Serial.print("\n");
        }
      }
    }
  }

  if (!is_mode_selected || is_mode_setup)
  { //Блокируем кнопки во время установки режима игры на всякий случай :)
    checkTeam1Button();
    checkTeam2Button();
    checkOKButton();
  }
}


//Установка стандартных параметров для определенного режима игры
void setupModeStandardConfig()
{
  switch (GAME_MODE)
  {
    case 1://Анриал торнамент таймер
      isCodeLength = true;// Код для этого режима не нужет
      isCodeSetup = true;// Код для этого режима не нужет
      isShowCodeTimeSetup = true;// Не отображаем меню для введения кода
      _TIME = 150.0; //временный параметр для игры в минутах
      TIME = _TIME * dividerMins; //2,5часа (время игры в мс)
      _TIME_DELAY_BEFORE_GAME = 15.0; // 15мин до запуска игры (временный параметр для старта в минутах)
      TIME_DELAY_BEFORE_GAME = _TIME_DELAY_BEFORE_GAME * dividerMins;//5 * 60 * 1000; //15mins время до старта в мс
      showMins = 60.0;

      myGLCD.clrScr();
      myGLCD.print("Настройка времени", CENTER, 5);
      myGLCD.print("до начала игры:", CENTER, 20);
      myGLCD.print(longintToChar(_TIME_DELAY_BEFORE_GAME), CENTER, 35);
      myGLCD.update();

      DELAY_STEP = 5.0;// mins
      TIME_STEP = 10.0;// mins
      SHOW_TIME_STEP = 5.0;// sec
      break;
    case 2:
      _TIME = 20.0; //временный параметр для игры в минутах
      TIME = _TIME * dividerMins; //2,5часа (время игры в мс)
      _TIME_DELAY_BEFORE_GAME = 15.0; // 15мин до запуска игры (временный параметр для старта в минутах)
      TIME_DELAY_BEFORE_GAME = _TIME_DELAY_BEFORE_GAME * dividerMins;//5 * 60 * 1000; //15mins время до старта в мс
      _TIME_SHOW_CODE = 30.0;
      TIME_SHOW_CODE = _TIME_SHOW_CODE * divider;//
      showMins = 60.0;

      myGLCD.clrScr();
      myGLCD.print("Настройка времени", CENTER, 5);
      myGLCD.print("игры в минутах:", CENTER, 20);
      myGLCD.print(longintToChar(_TIME), CENTER, 35);
      myGLCD.update();

      DELAY_STEP = 5.0;// mins
      TIME_STEP = 5.0;// mins
      SHOW_TIME_STEP = 5.0;// sec
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
  }

  delay(100);
  is_mode_setup = true;
}


void runMode_1()
{
  _summaryTime = currentTime - timeBegin;
  long tm;

  if (holdingTeam1)
  {
    tm = (summaryTimeTeam1 + _summaryTime) / 1000;
    //    if (tm != _prevSummaryTimeTeam1)
    //    {
    //    printDigits(tm / showMins, true);
    //    printDigits(summaryTimeTeam2 / 1000 / showMins, false);

    myGLCD.clrScr();
    myGLCD.print("Время команды 1: ", CENTER, 0);
    myGLCD.print(longToChar(tm / showMins / 1000), CENTER, 10);
    myGLCD.print("Время команды 2", CENTER, 24);
    myGLCD.print(longintToChar(summaryTimeTeam2 / 1000 / showMins), CENTER, 34);
    myGLCD.update();

    _prevSummaryTimeTeam1 = summaryTimeTeam1 / 1000;
    //    }
  } else if (holdingTeam2)
  {
    tm = (summaryTimeTeam2 + _summaryTime) / 1000;
    //    if (tm != _prevSummaryTimeTeam2)
    //    {
    //    printDigits(summaryTimeTeam1 / 1000 / showMins, true);
    //    printDigits(tm / showMins, false);

    myGLCD.clrScr();
    myGLCD.print("Время команды 1: ", CENTER, 0);
    myGLCD.print(longToChar(summaryTimeTeam1 / 1000 / showMins), CENTER, 10);
    myGLCD.print("Время команды 2", CENTER, 24);
    myGLCD.print(longintToChar(tm / showMins), CENTER, 34);
    myGLCD.update();

    _prevSummaryTimeTeam2 = summaryTimeTeam2 / 1000;
    //    }
  }

  //Переключение диодов, пищание пищалки
  if ( starBlinkTime + blinkDelay < currentTime)
  {
    starBlinkTime = millis();
    if (holdingTeam1 || holdingTeam2)
      beep(50);
    diodeBlink = !diodeBlink;
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPinN, LOW);
    if (diodeBlink)
    {
      if (holdingTeam1)
        digitalWrite(ledPin1, HIGH);
      else if (holdingTeam2)
        digitalWrite(ledPin2, HIGH);
      else
        digitalWrite(ledPinN, HIGH);
    }
  }
  delay(100);
}


void runMode_2()
{
  if (showingCode)
  {
    //пищание во время отображения кода
    if ( starBlinkTime + blinkDelay < currentTime)
    {
      starBlinkTime = millis();
      beep(50);
    }

    if (starTime + TIME_SHOW_CODE < currentTime)
    {
      myGLCD.clrScr();
      myGLCD.update();
      showingCode = false;
      starTime = millis();
    }
  } else {
    long et = (starTime + TIME - currentTime) / divider;

    myGLCD.clrScr();
    myGLCD.print("Время до", CENTER, 5);
    myGLCD.print("вывода кода:", CENTER, 20);
    myGLCD.print(longintToChar(et), CENTER, 35);
    myGLCD.update();

    if (starTime + TIME < currentTime)
    {
      myGLCD.clrScr();
      myGLCD.update();
      showingCode = true;
      starTime = millis();
      printCode(false);
    }
  }

}


void runMode_3()
{
}
void runMode_4()
{
}
void runMode_5()
{
}


void checkTeam1Button()
{
  if (!holdingTeam1)
    if (bouncerButton1.update())
      //если произошло событие
      if (bouncerButton1.read() == 0) {  //если кнопка нажата
        bouncerButton1.rebounce(buttonsDelay);    //повторить событие через 300мс

        if (!is_mode_selected) {
          GAME_MODE ++;
          if (GAME_MODE > GAME_COUNTS)
            GAME_MODE = 1;
          myGLCD.clrScr();
          myGLCD.print("Выберите режим", CENTER, 10);
          myGLCD.print(intToChar(GAME_MODE), CENTER, 30);
          myGLCD.update();
        } else if (!isCodeLength) {
          CODE_LENGTH ++;
          if (CODE_LENGTH > 8)
            CODE_LENGTH = 1;
          myGLCD.clrScr();
          myGLCD.print("Введите длину кода", CENTER, 10);
          myGLCD.print(longintToChar(CODE_LENGTH), CENTER, 30);
          myGLCD.update();
        } else if (!isCodeSetup) {
          _CODE_NUMBER ++;
          if (_CODE_NUMBER > 9)
            _CODE_NUMBER = 0;
          DIGIT1 = _CODE_NUMBER;
          printCode(false);
        } else if (!isSetupDelay) {
          _TIME_DELAY_BEFORE_GAME += DELAY_STEP;
          myGLCD.clrScr();
          myGLCD.print("Настройка времени", CENTER, 5);
          myGLCD.print("до начала игры:", CENTER, 20);
          myGLCD.print(longintToChar(_TIME_DELAY_BEFORE_GAME), CENTER, 35);
          myGLCD.update();
        } else if (!isSetupGameTime) {
          _TIME += TIME_STEP;
          myGLCD.clrScr();
          myGLCD.print("Настройка времени", CENTER, 5);
          myGLCD.print("игры в минутах:", CENTER, 20);
          myGLCD.print(longintToChar(_TIME), CENTER, 35);
          myGLCD.update();
        } else if (!isShowCodeTimeSetup) {
          _TIME_SHOW_CODE += SHOW_TIME_STEP;
          myGLCD.clrScr();
          myGLCD.print("Настройка времени", CENTER, 5);
          myGLCD.print("отображения кода в секундах:", CENTER, 20);
          myGLCD.print(longintToChar(_TIME_SHOW_CODE), CENTER, 35);
          myGLCD.update();
        } else if (isGameStarted) {
          if (holdingTeam2) //Добавляем время удержания команде 2
            summaryTimeTeam2 = summaryTimeTeam2 + _summaryTime;
          holdingTeam1 = true;
          holdingTeam2 = false;
          timeBegin = millis();
        }
      }
}


void checkTeam2Button()
{
  if (!holdingTeam2)
    if (bouncerButton2.update())
      //если произошло событие
      if (bouncerButton2.read() == 0) {  //если кнопка нажата
        bouncerButton2.rebounce(buttonsDelay);    //повторить событие через 300мс

        if (!is_mode_selected) {
          GAME_MODE --;
          if (GAME_MODE < 1)
            GAME_MODE = GAME_COUNTS;
          myGLCD.clrScr();
          myGLCD.print("Выберите режим", CENTER, 10);
          myGLCD.print(intToChar(GAME_MODE), CENTER, 30);
          myGLCD.update();
        } else if (!isCodeLength) {
          CODE_LENGTH --;
          if (CODE_LENGTH < 1)
            CODE_LENGTH = 8;
          myGLCD.clrScr();
          myGLCD.print("Введите длину кода", CENTER, 10);
          myGLCD.print(longintToChar(CODE_LENGTH), CENTER, 30);
          myGLCD.update();
        } else if (!isCodeSetup) {
          _CODE_NUMBER --;
          if (_CODE_NUMBER < 0)
            _CODE_NUMBER = 9;
          DIGIT1 = _CODE_NUMBER;
          printCode(false);
        } else if (!isSetupDelay) {
          _TIME_DELAY_BEFORE_GAME -= DELAY_STEP;
          if (_TIME_DELAY_BEFORE_GAME < 0)
            _TIME_DELAY_BEFORE_GAME = 0;
          myGLCD.clrScr();
          myGLCD.print("Настройка времени", CENTER, 5);
          myGLCD.print("до начала игры:", CENTER, 20);
          myGLCD.print(longintToChar(_TIME_DELAY_BEFORE_GAME), CENTER, 35);
          myGLCD.update();
        } else if (!isSetupGameTime) {
          _TIME -= TIME_STEP;
          if (_TIME < TIME_STEP)
            _TIME = TIME_STEP;
          myGLCD.clrScr();
          myGLCD.print("Настройка времени", CENTER, 5);
          myGLCD.print("игры в минутах:", CENTER, 20);
          myGLCD.print(longintToChar(_TIME), CENTER, 35);
          myGLCD.update();
        }  else if (!isShowCodeTimeSetup) {
          _TIME_SHOW_CODE -= SHOW_TIME_STEP;
          if (_TIME_SHOW_CODE < SHOW_TIME_STEP)
            _TIME_SHOW_CODE = SHOW_TIME_STEP;
          myGLCD.clrScr();
          myGLCD.print("Настройка времени", CENTER, 5);
          myGLCD.print("отображения кода в секундах:", CENTER, 20);
          myGLCD.print(longintToChar(_TIME_SHOW_CODE), CENTER, 35);
          myGLCD.update();
        } else if (isGameStarted) {
          if (holdingTeam1) //Добавляем время удержания команде 1
            summaryTimeTeam1 = summaryTimeTeam1 + _summaryTime;
          holdingTeam1 = false;
          holdingTeam2 = true;
          timeBegin = millis();
        }
      }
}

void checkOKButton()
{
  if (bouncerButtonN.update())
    //если произошло событие
    if (bouncerButtonN.read() == 0) {  //если кнопка нажата
      bouncerButtonN.rebounce(8 * buttonsDelay);    //повторить событие через 300мс

      if (!isGameStarted)
      {
        myGLCD.clrScr();
        myGLCD.update();
      }

      if (!is_mode_selected) {
        if (GAME_MODE == 1)
        {
          //Delay message
          myGLCD.clrScr();
          myGLCD.print("Настройка времени", CENTER, 5);
          myGLCD.print("до начала игры:", CENTER, 20);
          myGLCD.print(longintToChar(_TIME_DELAY_BEFORE_GAME), CENTER, 35);
          myGLCD.update();
        } else if (GAME_MODE == 2)
        {
          //Lenght message
          myGLCD.clrScr();
          myGLCD.print("Введите длину кода", CENTER, 10);
          myGLCD.print(longintToChar(CODE_LENGTH), CENTER, 30);
          myGLCD.update();
        }
        is_mode_selected = true;
      } else if (!isCodeLength) {
        printCode(true);
        isCodeLength = true;
      } else if (!isCodeSetup) {
        DIGIT1 = _CODE_NUMBER;
        CODE = buildCode(_CODE_NUMBER);

        if (CODE > pow(10, CODE_LENGTH - 1))
        {
          //Delay message
          myGLCD.clrScr();
          myGLCD.print("Настройка времени", CENTER, 5);
          myGLCD.print("до начала игры:", CENTER, 20);
          myGLCD.print(longintToChar(_TIME_DELAY_BEFORE_GAME), CENTER, 35);
          myGLCD.update();
          isCodeSetup = true;
        } else {
          _CODE_NUMBER = 0;
          printCode(false);
        }
      } else if (!isSetupDelay) {
        TIME_DELAY_BEFORE_GAME = _TIME_DELAY_BEFORE_GAME * dividerMins;
        isSetupDelay = true;

        //Time message
        myGLCD.clrScr();
        myGLCD.print("Настройка времени", CENTER, 5);
        myGLCD.print("до начала игры:", CENTER, 20);
        myGLCD.print(longintToChar(_TIME), CENTER, 35);
        myGLCD.update();
      } else if (!isSetupGameTime) {
        TIME = _TIME * dividerMins;
        isSetupGameTime = true;
        if (GAME_MODE == 1)
        {
          myGLCD.clrScr();
          myGLCD.print("Нажмите кнопку ОК", CENTER, 10);
          myGLCD.print("для старта игры игры:", CENTER, 35);
          myGLCD.update();
        } else if (GAME_MODE == 2)
        {
          //Time message
          myGLCD.clrScr();
          myGLCD.print("Настройка времени", CENTER, 5);
          myGLCD.print("до начала игры:", CENTER, 20);
          myGLCD.print(longintToChar(_TIME_DELAY_BEFORE_GAME), CENTER, 35);
          myGLCD.update();
        }
      } else if (!isShowCodeTimeSetup) {
        isShowCodeTimeSetup = true;
        myGLCD.clrScr();
        myGLCD.print("Нажмите кнопку ОК", CENTER, 10);
        myGLCD.print("для старта игры игры:", CENTER, 35);
        myGLCD.update();
      } else if (!isGameDelayStarted) {
        isGameDelayStarted = true;
        if (TIME_DELAY_BEFORE_GAME == 0)
          isGameStarted = true;
        myGLCD.clrScr();
        myGLCD.print("Время команды 1: ", CENTER, 0);
        myGLCD.print(longToChar(summaryTimeTeam1 / 1000 / showMins), CENTER, 10);
        myGLCD.print("Время команды 2", CENTER, 24);
        myGLCD.print(longToChar(summaryTimeTeam2 / 1000 / showMins), CENTER, 34);
        myGLCD.update();
        starTime = millis();
        starBlinkTime = millis();
      } else if (isGameStarted) {
        if (holdingTeam1 || holdingTeam2)
        {
          if (holdingTeam1) //Добавляем время удержания команде 1
            summaryTimeTeam1 = summaryTimeTeam1 + _summaryTime;
          if (holdingTeam2) //Добавляем время удержания команде 2
            summaryTimeTeam2 = summaryTimeTeam2 + _summaryTime;
          holdingTeam1 = false;
          holdingTeam2 = false;
          timeBegin = millis();
        }
      }
    }
}

//void printDigits(int value, boolean team1)
//{
//  int top = 0;
//  if (team1)
//    top = 24;
//  myGLCD.print("LCD5110_Graph", CENTER, top + 5);
//  myGLCD.update();
//}

//void printDigits8(long value)
//{
//  lc.clearDisplay(0);
//  int leftNumber = int(value / 10000);
//  int rightNumber = int(value % 10000);
//
//  if (leftNumber > 0)
//    printDigits(leftNumber, true);
//  printDigits(rightNumber, false);
//}

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

long buildCode(long value)
{
  return CODE * 10.0 + value;
}

void printCode(boolean endCode)
{
  char* code = "";
  if (DIGIT1 != -1)
    code += DIGIT1;
  if (DIGIT2 != -1)
    code += DIGIT2;
  if (DIGIT3 != -1)
    code += DIGIT3;
  if (DIGIT4 != -1)
    code += DIGIT4;
  if (DIGIT5 != -1)
    code += DIGIT5;
  if (DIGIT6 != -1)
    code += DIGIT6;
  if (DIGIT7 != -1)
    code += DIGIT7;
  if (DIGIT8 != -1)
    code += DIGIT8;

  if (endCode)
    code += _CODE_NUMBER;

  myGLCD.clrScr();
  myGLCD.print("КОД:", CENTER, 15);
  myGLCD.print(code, CENTER, 35);
  myGLCD.update();
}

char* longintToChar(long int value)
{
  char* v = "";
  v += value;
  return v;
}

char* intToChar(int value)
{
  char* v = "";
  v += value;
  return v;
}

char* longToChar(long value)
{
  char* v = "";
  v += value;
  return v;
}

//void moveCode()
//{
//  DIGIT8 = DIGIT7;
//  DIGIT7 = DIGIT6;
//  DIGIT6 = DIGIT5;
//  DIGIT5 = DIGIT4;
//  DIGIT4 = DIGIT3;
//  DIGIT3 = DIGIT2;
//  DIGIT2 = DIGIT1;
//  DIGIT1 = 0;
//}