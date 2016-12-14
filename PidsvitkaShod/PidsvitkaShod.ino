/* - Пiдсвытка може включатись одночасно i зверху i знизу сходячись в центрi
 * - Пiдсвiтка виключаэться аналогiчно зверху вниз або знизу вверх, або одночасно
 * вiд центру в рiзнi сторони якщо був рух з обох сторiн
 * - Якщо спрацьовуэ датчик руху, ресетуэмо час затримки
 * - При виключеннi свiтла i спрацюваннi датчикыв руху по серединi, включаэмо дiоди по черзi
 * кратнi "3", тобто за 3 кроки включаються всi дiоди
 * - Час очiкування руху DELAY_TIME = 10000;//10 секунд
 * - Час мiж включеннями дiодiв STEP_TIME = 400 + 100delay;//0.5 секунди
 */

#include "Ultrasonic.h"

//Ультразвуковi Датчики вiдстанi
Ultrasonic ultrasonic1(30, 31); //I поверх
Ultrasonic ultrasonic2(32, 33); //II поверх

//Датчики руху
int peerPin1 = 34;//низ
int peerPin2 = 35;//центр
int peerPin3 = 36;//середина

int peerVal1 = 0;
int peerVal2 = 0;
int peerVal3 = 0;

int startTime = 0;//Час вiд включення 1го дiода
int STEP_TIME = 400;// ms, Затримка мiж включенням дiодiв 400 + 100delay = 500ms
int motiontTime = 0;//
int DELAY_TIME = 10000;//10s

boolean motionStarted = false;
boolean turnOffStarted = false;
boolean leedsChanged = false;

boolean startFromBottomInProgress = false;
boolean startFromTopInProgress = false;
boolean startFromMiddleInProgress = false;
boolean stopFromBottomInProgress = false;
boolean stopFromTopInProgress = false;
boolean stopFromMiddleInProgress = false;
boolean fromBottom = false;
boolean fromTop = false;

//Кылькысть сходинок
int N = 13;
int leeds[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//iндекси включення для сходинок
int b = 0;// перша сходинка
int t = 12;// остання сходинка (= N - 1)
int m = 5;// центральна сходинка (= N/2 - 1)
int distance = 10;

//константи для сходинок
int B = 0;// перша сходинка
int T = 12;// остання сходинка (= N - 1)
int M = 5;// центральна сходинка (= N/2 - 1)

void setup() {
  for(int j=0; i<N; j++)
  {
    digitalWrite(j, LOW);
    pinMode(j, OUTPUT);
  }
  pinMode(peerPin1, INPUT);
  pinMode(peerPin2, INPUT);
  pinMode(peerPin3, INPUT);

  Serial.begin (9600);

  delay(60000);//затримка 60сек для калібрування датчика руху
}

void loop() {
  int cm1 = ultrasonic1.Ranging(CM);
  int cm2 = ultrasonic2.Ranging(CM);
  Serial.print(cm1);
  Serial.println(" cm");
  Serial.print(cm2);
  Serial.println(" cm");

  peerVal1 = digitalRead(peerPin1);  // зчитуэмо значення з датчика руху
  peerVal2 = digitalRead(peerPin2);  // зчитуэмо значення з датчика руху
  peerVal3 = digitalRead(peerPin3);  // зчитуэмо значення з датчика руху

  if (cm1 < distance && cm1 != 0)
  {
    //При кожному спрацюваннi будь-якого датчика обновляэмо час руху
    motiontTime = millis();
    if (!startFromMiddleInProgress)
    {
      b = B;
      fromBottom = true;
      startFromBottomInProgress = true;
      turnOnFromBottom();
    }
  }

  if (cm2 < distance && cm2 != 0)
  {
    //При кожному спрацюваннi будь-якого датчика обновляэмо час руху
    motiontTime = millis();
    if (!startFromMiddleInProgress)
    {
      t = T;
      fromTop = true;
      startFromTopInProgress = true;
      turnOnFromTop();
    }
  }

  if (peerVal1 == HIGH || peerVal2 == HIGH || peerVal3 == HIGH) {
    //При кожному спрацюваннi будь-якого датчика обновляэмо час руху
    motiontTime = millis();
    if (!startFromBottomInProgress && !startFromTopInProgress)
    {
      b = M;
      t = M;
      startFromMiddleInProgress = true;
      turnOnFromMiddle();
    }
  }

  int currentTime = millis();
  //Якщо не було руху, перевiряэмо час чи не пора виключити свiтло
  //Якщо свiтло почали виключати, то нiчого не робимо
  if (currentTime - motiontTime >= DELAY_TIME && !turnOffStarted)
  {
    turnOffStarted = true;
    //Рух з обох сторын, виключаэмо выд центру
    if (fromBottom && fromTop)
    {
      b = M;
      t = M;
      turnOffFromMiddle();
    }//Рух почався з верху, виключаэмо з верху
    else if (fromBottom)
    {
      b = B;
      turnOffFromBottom();
    }//Рух почався з низу, виключаэмо з низу
    else if (fromTop)
    {
      t = T;
      turnOffFromTop();
    }
  }

  //Якщо почали виключати свiтло запускаэмо вiдповiдний режим
  if (turnOffStarted)
  {
    if (startFromBottomInProgress)
    {
      turnOffFromBottom();
    } else if (startFromTopInProgress)
    {
      turnOffFromTop();
    } else if (startFromMiddleInProgress)
      turnOffFromMiddle();
  }

  if (leedsChanged)
    setStates();

  delay(100);
}

void turnOnFromBottom()
{
  startFromBottomInProgress = true;
  if (leeds[b] == 1)
  {
    startFromBottomInProgress = false;
  } else {
    leeds[b] = 1;
    b++;
    if (b >= N - 1)
      startFromBottomInProgress = false;
  }
  leedsChanged = true;
}

void turnOffFromBottom()
{
  stopFromBottomInProgress = true;
  if (leeds[b] == 0)
  {
    stopFromBottomInProgress = false;
  } else {
    leeds[b] = 0;
    b++;
    if (b >= N - 1)
      stopFromBottomInProgress = false;
  }
  leedsChanged = true;
}

void turnOnFromTop()
{
  startFromTopInProgress = true;
  if (leeds[t] == 1)
  {
    startFromTopInProgress = false;
  } else {
    leeds[t] = 1;
    t--;
    if (t <= 0)
      startFromTopInProgress = false;
  }
  leedsChanged = true;
}

void turnOffFromTop()
{
  stopFromTopInProgress = true;
  if (leeds[t] == 0)
  {
    stopFromTopInProgress = false;
  } else {
    leeds[t] = 0;
    t--;
    if (t <= 0)
      stopFromTopInProgress = false;
  }
  leedsChanged = true;
}

void turnOnFromMiddle()
{
  startFromMiddleInProgress = true;

  if (b < N - 1 && leeds[b] != 1)
  {
    leeds[b] = 1;
    b++;
  }

  if (t > 0 && leeds[t] != 1)
  {
    leeds[t] = 1;
    t--;
  }

  if (b >= N - 1 && t <= 0)
    startFromMiddleInProgress = false;

  leedsChanged = true;
}

void turnOffFromMiddle()
{
  stopFromMiddleInProgress = true;

  if (b < N - 1 && leeds[b] != 0)
  {
    leeds[b] = 0;
    b++;
  }

  if (t > 0 && leeds[t] != 0)
  {
    leeds[t] = 0;
    t--;
  }

  if (b >= N - 1 && t <= 0)
    stopFromMiddleInProgress = false;
  leedsChanged = true;
}

void setStates()
{
  for (int i; i < N; i++)
  {
    digitalWrite(i, leeds[i]);
  }
  leedsChanged = false;
}

