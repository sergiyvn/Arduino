#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 7);

//Флаги
boolean PIDAT = 0, ALARM = 0, FB = 0;

//кнопки
int UP = 17, DN = 18, SET = 19;
int reg, er, zad;
boolean out = 0; //управление симистором
double I, Kp = 1, Ki = 1, Kd = 1, pre_err, err;
double temp = 0; //текущая температура
double ust = 22; //уставка
int ten = 13; //выход на семистр (нагреватель)
unsigned long PIDMillis;//сброс таймера
unsigned long tempMillis;//сброс таймера для замера темп
unsigned long menuMillis;//сброс таймера для возврата меню
int m = 0; //переменная меню

void setup() {
  pinMode(UP, INPUT); //кнопка вверх
  pinMode(DN, INPUT); //кнопка вниз
  pinMode(SET, INPUT); //кнопка SET
  pinMode(ten, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(0, zero_crosss_int, RISING);//внешнее прерывание ZC
  lcd.begin(16, 2);
  lcd.setCursor(1, 0);
  lcd.print("Termostat v2.0");
  delay(2000);
  lcd.clear();
  //AutoPIDSet();автонастройка при включении
}

void loop() {
  unsigned long currentMillis = millis();
  //каждую секунду мерим температуру
  if (currentMillis - tempMillis >= 1000) {

    temp_lm35();

    tempMillis = currentMillis;
  }
  //каждые 250мс
  if (currentMillis - PIDMillis >= 250) {
    PID();//расчитываем ПИД
    PIDMillis = currentMillis;
    key();//проверяем кнопки
    if (currentMillis - menuMillis >= 10000) {
      m = 0;
    }
    menu();//выводим меню
  }


}

//регулятор ПИД
// void PID() расчитывает управляющее воздействие на нагреватель в %
// (глобальная переменная zad), далее эта переменная берется в
// функции ResOUT() которая вызываеться от внешнего прерывания детектора
// нуля для расчета по алгоритму Брезенхема пропустить полуволну синусойды
// питающей тен сети или нет.
void PID() {
  err = ust - temp;
  I = I + err;
  zad = Kp * err + (Ki * I * 0.25) + (Kd * (err - pre_err) / 0.25);
  pre_err = err;

  if (zad < 0) {
    zad = 0;
    I = 0;
  }
  if (zad > 100) {
    zad = 100;
  }
}

//Замер температуры
void temp_lm35() {
  temp = analogRead(0);
  temp = temp * 5 / 1024 * 100;
}

//Внешнее прерывание детектора нуля
void zero_crosss_int()
{
  ResOut();//Вызов функции управления симистором по алгоритму Брезенхема
}

//управление симистором
void ResOut() {
  delayMicroseconds(1230);//для попадания в ноль
  reg = zad + er;
  if (reg < 50) {
    out = 0;
    er = reg ;
  }
  else {
    out = 1;
    er = reg - 100;
  }
  digitalWrite(ten, out);
}
//Авто настройка коэффициентов ПИД
void AutoPIDSet() {
  //тут будет автонастройка
}

void menu() {
  lcd.clear();
  switch (m) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print(" Temp  Ust  Pwr%");
      lcd.setCursor(0, 2);
      lcd.print(temp);
      lcd.setCursor(6, 2);
      lcd.print(ust);
      lcd.setCursor(13, 2);
      lcd.print(zad);
      break;
    case 10:
      lcd.setCursor(0, 0);
      lcd.print("SET Temperature");
      lcd.setCursor(6, 2);
      lcd.print(ust);
      break;
    case 20:
      lcd.setCursor(0, 0);
      lcd.print("PID auto tune");
      break;
    case 30://коды ошибок
      lcd.setCursor(5, 0);
      lcd.print("ALARM");
      lcd.setCursor(7, 2);
      lcd.print(ALARM);
      break;
  }
}
void key() {
  if (digitalRead(SET) == LOW && FB == 1) {
    FB = 0;  //сброс защелки кнопок и сброс таймера автовозврата на основной экран
    menuMillis = millis();
  }
  if (digitalRead(UP) == LOW && FB == 1) {
    FB = 0;
    menuMillis = millis();
  }
  if (digitalRead(DN) == LOW && FB == 1) {
    FB = 0;
    menuMillis = millis();
  }

  if (digitalRead(SET) == HIGH && FB == 0) {
    key_SET();
  }
  if (digitalRead(UP) == HIGH && FB == 0) {
    key_UP();
  }

  if (digitalRead(DN) == HIGH && FB == 0) {
    key_DN();
  }
}

void key_SET() {
  m = m + 10;
  FB = 1;
  if (m > 30) {
    m = 0;
  }
}

void key_UP() {
  if (m == 10) {
    ust = ust + 0.1;
    FB = 1;
    if (ust > 80) {
      ust = 80;
    }
  }
}

void key_DN() {
  if (m == 10) {
    ust = ust - 0.1;
    FB = 1;
    if (ust < 6) {
      ust = 5;
    }
  }
  if (m == 20) {
    PIDAT = 1;
    AutoPIDSet();
  }
}

