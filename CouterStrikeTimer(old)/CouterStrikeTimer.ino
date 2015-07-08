//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <leOS.h>  
leOS myOS;  //создать экземпляр класса 

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int beepPin = 9;

int interval=-1;
boolean isMinTime1 = false;
boolean isMinTime2 = false;
int minTime1 = 0;
int minTime2 = 0;
int timerSpeed = 1;
int timerDelay = 1000;

char current = 'G';
char _current = 'G';

int rows[]={11,8,7,6}; //указываем пины строк
int cols[]={5,4,3,2}; //и столбцов

int a1 = -1;
int a2 = -1;
int a3 = -1;
int a4 = -1;

int b1 = -1;
int b2 = -1;
int b3 = -1;
int b4 = -1;

boolean proccessing = false; //не запускать дважды подряд зажатую кнопку
boolean wasBoom = false; //взорвалась ли бомба?
boolean beeping = false;
boolean buttonPressed = false; //для запоминания последней нажатой кнопки
char _lastKey = 'G';

char simbols[][2]={
  {0,'1'},
  {1,'2'},
  {2,'3'},
  {3,'A'},
  {4,'4'},
  {5,'5'},
  {6,'6'},
  {7,'B'},
  {8,'7'},
  {9,'8'},
  {10,'9'},
  {11,'C'},
  {12,'*'},
  {13,'0'},
  {14,'#'},
  {15,'D'}
};

char ints[][2]={
  {'1',1},
  {'2',2},
  {'3',3},
  {'4',4},
  {'5',5},
  {'6',6},
  {'7',7},
  {'8',8},
  {'9',9},
  {'0',0}
};


void setup()
{
  lcd.init();                     // инициализация LCD 
  lcd.backlight();                // включаем подсветку
  lcd.clear();                    // очистка дисплея
  lcd.setCursor(1, 0);            // устанавливаем курсор на 1 строку, 4 символ
  lcd.print("Starting...");          // вывод надписи
  lcd.setCursor(1, 1);            // устанавливаем курсор на 2 строку, 1 символ

  pinMode(9, OUTPUT); //пищалка
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(10, OUTPUT); //красный диод
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT); //бомба
  
  
  beep(50);
  beep(50);
  beep(50);
  delay(1000);
  lcd.clear();                    // очистка дисплея
  lcd.setCursor(1, 0);            // устанавливаем курсор на 1 строку, 4 символ
  lcd.print("Enter time:");
   lcd.setCursor(1, 1);            // устанавливаем курсор на 1 строку, 4 символ
  lcd.print("Enter time:");

//  myOS не тянет 4 задачи :(   
  myOS.begin();
  myOS.addTask(checkButtonsTask, 1000/timerSpeed);//добавить задачу с интервалом выполнения через xxx мс 
  myOS.addTask(beepTask, 500/timerSpeed);//добавить задачу с интервалом выполнения через xxx мс 
  myOS.addTask(ledTask, 500/timerSpeed); //переключение диода пол сек
//  myOS.addTask(blinkDots, 500); //переключение диода пол сек

}
 
void loop()
{
//  delay(200);
  _current = keysLoop();
  
//надо добавить запоминание последней нажатой кнопки... boolean buttonPressed = false;

  if(!proccessing){
    if(interval == -1)
    {
     checkButtons();
    } else if(interval == 1 || interval == 0 ){
      if(interval == 1)
        checkTime(); 
      if(_current == '*')
         interval = -1;
    } else {
      if(_current == 'D'){
         interval = -1;
         lcd.clear();
         lcd.setCursor(1, 0);            // устанавливаем курсор на 1 строку, 4 символ
         lcd.print("Congratulations ");
         lcd.setCursor(2, 1);            // устанавливаем курсор на 1 строку, 4 символ
         lcd.print("Bomd stopped");
         analogWrite(beepPin, 0);
         beeping = false;
         digitalWrite(10, LOW);
     } else {
//      Если балуемся с кнопками, ускоряем время в 2е
        if(_current != 'G' && timerSpeed < 4){
          timerSpeed = timerSpeed*2;
          myOS.modifyTask(checkButtonsTask, 1000/timerSpeed);//добавить задачу с интервалом выполнения через xxx мс 
          myOS.modifyTask(beepTask, 500/timerSpeed);//добавить задачу с интервалом выполнения через xxx мс 
          myOS.modifyTask(ledTask, 500/timerSpeed); //переключение диода пол сек
        }
        checkTime();  
      }  
    } 
  }
}

void checkButtonsTask()
{
//  changePin(10, 500/timerSpeed); 
}

void beepTask()
{
 if( interval > 1){
//   beep(200/timerSpeed); 
     if(beeping){
        analogWrite(beepPin, 0);
        beeping = false;
     } else {
        analogWrite(beepPin, 20);
        beeping = true;
     }
 } else if (interval == 1) {
//    beep(50); 
 }
}

void ledTask()
{
  if( interval > 1){
    if(digitalRead(10) == LOW)
      digitalWrite(10, HIGH);
    else
      digitalWrite(10, LOW);
//    changePin(10, 5000/timerSpeed);
  } else //if (interval == 1) {
//    changePin(10, 5000); 
  digitalWrite(10, LOW);
 //}
}

//мигание двоеточия в часах
void blinkDots()
{
  if( interval > 1){
    lcd.setCursor(10, 0);
    lcd.print(" "); 
  }
}


void checkButtons(){
  proccessing = true;
  
  if(_current == '0' || _current == '1' || _current == '2' || _current == '3' || _current == '4' 
  || _current == '5' || _current == '6' || _current == '7' || _current == '8' || _current == '9')
  {
    delay(300);
    current = _current;
    if(!isMinTime1){
      if(FindInt(current) == 0){
        beep(200);
      } else {
        isMinTime1 = true;
        minTime1 = FindInt(current);
        lcd.setCursor(14, 0); 
        lcd.print(minTime1);
        lcd.setCursor(0, 1); 
        lcd.print("Confirm #");
      }
    } else if(!isMinTime2){
      isMinTime2 = true;
      minTime2 = FindInt(current);
      lcd.setCursor(15, 0); 
      lcd.print(minTime2);
    } else {
      beep(200);
    }
  } else if(_current == '*')
  {
    timerSpeed = 1;
    wasBoom = false;
    isMinTime1 = false;
    isMinTime2 = false;
    minTime1 = 0;
    minTime2 = 0;
    lcd.clear();
    lcd.setCursor(1, 0);   
    lcd.print("Enter time:");
//    myOS.modifyTask(checkButtonsTask, 1000/timerSpeed);//добавить задачу с интервалом выполнения через xxx мс 
//    myOS.modifyTask(beepTask, 500/timerSpeed);//добавить задачу с интервалом выполнения через xxx мс 
//    myOS.modifyTask(ledTask, 500/timerSpeed); //переключение диода пол сек
  } else if(_current == '#')
  {
     //set interval
     if(!isMinTime2){
       interval = minTime1*60;
       beep(50);
       beep(50);
       beep(50);
       lcd.clear();
       lcd.print("Time:");
       printTime(interval, 6 ,0);
     } else if(!isMinTime1 && !isMinTime2) {
       beep(200);
     } else {
       interval = (minTime1*10 + minTime2)*60;//set to minutes
       beep(50);
       beep(50);
       beep(50);
       lcd.clear();
       lcd.print("Time:");
       printTime(interval, 6 ,0);
     }
  }
  
  proccessing = false;
}

void checkTime()
{
 if( interval > 1)
 {
   timerDelay = 1000/timerSpeed;
   delay(timerDelay);
   interval--;
//   beep(200/timerSpeed);
//   lcd.setCursor(7, 0);
//   lcd.print("  ");
//   if(interval > 9){
//    lcd.setCursor(7, 0);
//   } else {
//     lcd.setCursor(8, 0);
//   }
     printTime(interval, 6 ,0);//выводит ежисекундно время в формате ххх:хх
//   lcd.setCursor(0, 1);
//   lcd.print(timerSpeed);   
//   lcd.setCursor(5, 1);
//   lcd.print(timerDelay); 
//   changePin(10, 500);
 }else
 {
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("    Time out");
   lcd.setCursor(0, 1);
   lcd.print(" *** Boooom *** ");
   beep(50);
   if(!wasBoom)
     doBoom();
 }
}

void doBoom()
{
  wasBoom = true;
  digitalWrite(11, LOW); 
  digitalWrite(13, HIGH);  
  delay(2000);              
  digitalWrite(13, LOW);
}

void changePin(int value, int del)
{
  digitalWrite(value, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(value, LOW);    // turn the LED off by making the voltage LOW
  delay(500);  
}

void beep(unsigned char delayms){
  analogWrite(beepPin, 20);      // значение должно находится между 0 и 255
                           // поэкспериментируйте для получения хорошего тона
  delay(delayms);          // пауза delayms мс
  analogWrite(beepPin, 0);       // 0 - выключаем пьезо
  delay(delayms);          // пауза delayms мс   
}

char keysLoop()
{
 char c = 'G';
 for(int i=0;i<4;i++){ //цикл для перехода по всем строкам
    digitalWrite(rows[i], HIGH); //подаем на текущую строку высокий уровень
    for(int j=0;j<4;j++){ //цикл для переходов по всем столбцам
      if(digitalRead(cols[j])==HIGH){ //если уровень высокий, то кнопка нажата
        Serial.print(FindKey(4*i+j)); //выводим в терминал код нажатой кнопки
        c = FindKey(4*i+j);
      }
    }
    digitalWrite(rows[i], LOW);  //выключаем высокий уровень для пройденной строки
  }
  return c;
}

//функция возвращающая код нажатой кнопки
char FindKey(unsigned int keyCode){
  for(int i=0;i<16;i++){
    if(simbols[i][0]==keyCode){
      return simbols[i][1];
    }
  }
  return '-'; //если код на найден возвращаем ‘-‘
}

int FindInt(unsigned int keyCode){
  for(int i=0;i<16;i++){
    if(ints[i][0]==keyCode){
      return ints[i][1];
    }
  }
  return -1; //если код на найден возвращаем 100
}


//Не используеться
/*
char getFormattedTime(int value)
{
  int minutes = value/60;
  int seconds = value - minutes*60;
  char m = '00';
  char s = '00';
  
  if(minutes > 99){
    m = char(minutes);
  } else if (minutes > 9){
    m = ' ' + char(minutes);
  } else {
    m =  ' 0' + char(minutes);
  }
  
  if (seconds > 9){
    s = ' ' + char(seconds);
  } else {
    s =  ' 0' + char(seconds);
  }
  
  return m + ':' + s;
}*/


//Не используеться
/*
char getMinutes(int value)
{
  int minutes = value/60;
  char m = '00';
  
  if(minutes > 99){
    m = char(minutes);
  } else if (minutes > 9){
    m = ' ' + char(minutes);
  } else {
    m =  ' 0' + char(minutes);
  }
  return m;
}

char getSeconds(int value)
{
  int minutes = value/60;
  int seconds = value - minutes*60;
  char s = '00';
   
  if (seconds > 9){
    s = ' ' + char(seconds);
  } else {
    s =  ' 0' + char(seconds);
  }
  
  return s;
}*/

void printTime(int value, int x, int y)
{
  int m = value/60;
  int s = value - m*60; 

  lcd.setCursor(x, y);
  if(m > 99){
    lcd.print(m);
  } else if (m > 9){
  lcd.setCursor(x+1, y);
    lcd.print(m);
  } else {
    lcd.setCursor(x+1, y);
    lcd.print('0');
    lcd.print(m);
  }
  
  lcd.print(':');
  
  if (s > 9){
    lcd.print(s);
  } else {
    lcd.print('0');
    lcd.print(s);
  }
}
