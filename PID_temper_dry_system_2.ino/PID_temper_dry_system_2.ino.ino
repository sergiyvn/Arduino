#include "powerControl.h"
#include "beeper.h"
#include "ads1118.h"
#include "usart.h"
#include "converter.h"

//Tемпература хранится как uint == градусы*10*256 (т.е. с точностью до 1/2560 градуса)

//  Константы

// Время
#define TIM_01Sec    5
#define TIM_02Sec   10
#define TIM_05Sec   25
#define TIM_1Sec    50
#define TIM_2Sec    100
#define TIM_1Min    (TIM_1Sec*60)
#define TIM_2Min    (TIM_1Sec*120)
#define TIM_3Min    (TIM_1Sec*180)

#define TIM_MIN_WARMUP  TIM_02Sec

// Ёто время в секундах
#define TIM_DRY     120
#define TIM_PREWARMUP   60

// Температура
#define TM_005C     FROM_DEGREE(0.05)
#define TM_02C      FROM_DEGREE(0.2)
#define TM_05C      FROM_DEGREE(0.5)
#define TM_2C     FROM_DEGREE(2.0)
#define TM_5C     FROM_DEGREE(5.0)
#define TM_20C     FROM_DEGREE(20.0)
#define TM_PRESET     FROM_DEGREE(100.0)

#define TM_WARMUPC    (FROM_DEGREE(1.4)/TIM_1Sec)   //WORK: Нужно вычислить коэффициент (пока 1 град/секунда)
#define TM_WARMDOWNC  (FROM_DEGREE(0.005)/TIM_1Sec) //WORK: Нужно вычислить коэффициент (похоже что 0,09 град/секунда)

//Задана температура
#define TM_PRESET      TM_20C

///////////////////////////////////////////////////////////////////
powerControl::PCS_STAGES powerControl::state; //что сейчас делает система

// Данные от датчика температуры
uint powerControl::tempValue; // Временный счетчик для складывания промежуточных результатов
uint powerControl::count; // счетчик кол-ва сложенных отсчетов ADC

uint powerControl::realValues[8]; // Усредненное значение от 64 отсчетов (считаем что нормально сгладили)
uint powerControl::realValuePos;  // Где лежит текущее значение
uint powerControl::sumValue; // Сумма 8 последних значений
uint powerControl::avgValue; // Усредненное плавающее значение от 8 последних отсчетов (используется для контроля падения или возрастания температуры)

///////////////////////////////////////////////////////////////////
// Расчетные данные от контроллера энергии (прогноз)
volatile uint powerControl::internalValue;

///////////////////////////////////////////////////////////////////
// Данные для управления “ЁЌ-ом
volatile uint powerControl::PWMCountDown; // счетчик отсчета периода времени включения “ЁЌ-а

///////////////////////////////////////////////////////////////////
// Счетчики времени
volatile int powerControl::currStateCountDown;// счетчик отсчета текущего шага
volatile int powerControl::currTimer; // Таймер обратного отсчета с учетом дополнительного времени на сушку
volatile int powerControl::timeBeforeCold; //Время до начала остывания. Отмеряет защитный интервал при начале нагрева.

///////////////////////////////////////////////////////////////////
uint powerControl::targetValue; // Целевая температура
uint powerControl::targetTime; // Сколько надо выдержать целевую температуру

void powerControl::init()
{
  state = PCS_STOPED;

  count = 0;

  tempValue = 0;

  PWMCountDown = 0;
  currStateCountDown = 0;
  timeBeforeCold = 0;

  realValuePos = 0;
  avgValue = 0;
  sumValue = 0;
  realValues[0] = 0;
  realValues[1] = 0;
  realValues[2] = 0;
  realValues[3] = 0;
  realValues[4] = 0;
  realValues[5] = 0;
  realValues[6] = 0;
  realValues[7] = 0;

  internalValue = 0;

  //Настройка портов
  //  Конфигурируем CRH регистры.
  PORT_POWER->CRL &= ~(0xF << (PIN_POWER - 0) * 4); // Сбрасываем биты CNF для бита 15. Режим 00 - Out
  PORT_POWER->CRL |= (0x2 << (PIN_POWER - 0) * 4);  // Выставляем бит MODE1 для 15-ого пина. Режим MODE10 = Max Speed 2MHz

  _powerOff();
}


// Уведомление от таймера (делаем что-то быстрое)
void powerControl::onTick()
{
  //уменьшаем или увеличиваем внутренную температуру (для предсказания температуры)
  if (timeBeforeCold > 0)
  {
    timeBeforeCold--;

    if (PWMCountDown > 0)
    {
      PWMCountDown--;
      //Для контроля PWM (отключение в нужный момент)
      if (PWMCountDown <= 0) _powerOff();

      internalValue += TM_WARMUPC;
    }
  }

  int value = convADS1118::warmDown(realValues[realValuePos]);

  if (internalValue > value)
    internalValue -= value;
}

// Тики каждую секунду
void powerControl::onSecondTick()
{
  //Мигание светодиодами каждую секунду (в зависимости от статуса или точками на экране или диодами наверху...)
  /*switch (state)
  {
    case PCS_STOPED:
    case PCS_STARTING:
      return;
    case PCS_WARMUP_TO_DRY:
      tm1638::LEDInverse(0);
      break;
    case PCS_DRY:
      tm1638::LEDInverse(1);
      break;
    case PCS_PREWARMUP_TO_TARGET:
      tm1638::LEDInverse(2);
      break;
    case PCS_PREWARMUP_WAIT:
      tm1638::LEDInverse(3);
      break;
    case PCS_WARMUP_TO_TARGET:
      tm1638::LEDInverse(4);
      break;
    case PCS_WAIT_FOR:
      tm1638::LEDInverse(5);
      break;
    case PCS_COOL_DOWN:
      tm1638::LEDInverse(0);
      tm1638::LEDInverse(1);
      tm1638::LEDInverse(2);
      tm1638::LEDInverse(3);
      tm1638::LEDInverse(4);
      tm1638::LEDInverse(5);
      tm1638::LEDInverse(6);
      break;
  }*/

  //Уменьшаем счетчик времени
  if (currStateCountDown > 0)
  {
    //Идея: счетчик времени есть только в те моменты, когда мы ждем (TIM_DRY, TIM_PREWARMUP...). и только в эти моменты мы уменьшаем основной счетчик
    if (currTimer > 0) currTimer--;

    //ƒля контроля профиля температуры по времени (уменьшаем счетчик очередного периода)
    currStateCountDown--;
  }
}

//Функция для проверки необходимости включения подогрева и расчета времени включения
bool powerControl::_checkWarmUp(uint realValue, uint intValue, uint targetValue)
{
  if (realValue < targetValue) //если еще не нагрели, то пытаемся греть...
  {
    if (timeBeforeCold <= 0 && intValue < targetValue) // если еще не греем, то считаем, насколько греть...
    {
      //посчитать насколько запустить нагрев (не менее 0.1 сек)...
      int tmpTime = (targetValue - intValue + TM_02C) / TM_WARMUPC + TIM_01Sec;

      if (tmpTime >= TIM_MIN_WARMUP)
      {
        PWMCountDown = tmpTime;

        if (tmpTime < TIM_2Sec)
          timeBeforeCold = tmpTime + TIM_2Sec;
        else
          timeBeforeCold = tmpTime + 1;

        _powerOn();
      }
    }

    return (realValue > targetValue - TM_05C); //если пол-градуса недогрели, то не важно
  }
  return true; //Нагрели...
}


void powerControl::forceUpdate()
{
  if (PCS_STOPED == state && currTimer <= 0)
    gui::updateInfo(targetTime, realValues[realValuePos]);
  else
    gui::updateInfo(currTimer, realValues[realValuePos]);
}


// Проверяем готовность новых данных и возвращаем текущие (возможно обновленные) данные
uint powerControl::_tryAddADCData()
{
  if (!ads1118::isDataReady()) return (realValues[realValuePos]);

  tempValue += ads1118::getTemp();

  count++;

  if (count < 32) return (realValues[realValuePos]);

  tempValue <<= 3;

  realValuePos++;

  realValuePos &= 7;

  // Вычисляем среднюю температуру по больнице
  sumValue -= realValues[realValuePos];
  sumValue += tempValue;
  avgValue = sumValue >> 3;
  realValues[realValuePos] = tempValue;

  count = 0;

  tempValue = 0;

  return (realValues[realValuePos]);
}

// Время от основного цикла, тут можно работать долго (100-200 тысяч тактов)...
void powerControl::checkData()
{
  // Идея:
  // 0) есть реальная температура от датчика, внутренняя температура на основе предсказания, и целевая температура
  // 1) Внутрення никогда не может быть меньше реальной.
  // в режиме стоп: внутренняя == реальная
  // в режиме работы:
  // если подогрев включен или реальная температура не падает, то внутреення температура вычисляется по предсказаниям.
  // иначе (подогрев выключен и реальная температруа уменьшается): внутренняя == реальная

  uint intValue = internalValue; // внутреннаяя температура (копия, т.к. она обновляется асинхронно в другом потоке)
  uint realValue = _tryAddADCData(); // реальная температура

  if (isStoped() || intValue < realValue || (realValue < avgValue - TM_005C && timeBeforeCold <= 0))
  {
    internalValue = realValue;
    intValue = realValue;
  }

  // Тут у нас есть целевая текущая и расчетная температура. Строим алгоритм...

  if ((realValue >= targetValue || intValue >= targetValue) && PWMCountDown > 0) //Перегревать нельзя...
  {
    _powerOff();
  }

  switch (state)
  {
    case PCS_STOPED:
      return;
    case PCS_STARTING: // Начинаем цикл
      if (realValue > targetValue + TM_2C)// Если печка перегрета, то запускать нельзя...
      {
        stop();

        beeper::On(3200);
        break;
      }
      else
      {
        //Начинаем все отсчеты заново
        currStateCountDown = 0;
        //если это продолжение остановленной работы, то продолжаем
        if (realValue >= targetValue - TM_5C && currTimer > 0)
        {
          state = PCS_WARMUP_TO_TARGET;
          tm1638::LEDOn(0);
          tm1638::LEDOn(1);
          tm1638::LEDOn(2);
          tm1638::LEDOn(3);
          tm1638::LEDOn(4);
          break;
        }
        //если мы греем меньше заданого(100градусов) или уже нагрето больше заданого(100градусов), 
        //то сразу переключаемся на предварительный нагрев
        else if (targetValue < TM_PRESET || realValue > TM_PRESET)
        {
          state = PCS_PREWARMUP_TO_TARGET;
          tm1638::LEDOn(0);
          tm1638::LEDOn(1);
          tm1638::LEDOn(2);
          currTimer = targetTime + TIM_PREWARMUP;
          break;
        }
        state = PCS_WARMUP_TO_DRY;
        tm1638::LEDOn(0);
        currTimer = targetTime + TIM_DRY + TIM_PREWARMUP;
      }
    case PCS_WARMUP_TO_DRY:
    case PCS_DRY:
      if (PCS_DRY != state || currStateCountDown > 0) //если не цикл поддержания температуры, или время не вышло, поддерживаем температуру...
      {
        if (_checkWarmUp(realValue, intValue, TM_PRESET)) //если еще не нагрели, то пытаемся греть...
        {
          if (PCS_DRY != state) //если температура нужная и не поддерживаем температуру, то начинаем поддерживать температуру заданное время
          {
            state = PCS_DRY;
            currStateCountDown = TIM_DRY;
            tm1638::LEDOn(0);
            tm1638::LEDOn(1);
          }
        }
        break;
      }
      state = PCS_PREWARMUP_TO_TARGET;
      tm1638::LEDOn(1);
      tm1638::LEDOn(2);
    case PCS_PREWARMUP_TO_TARGET:
    case PCS_PREWARMUP_WAIT:
      if (PCS_PREWARMUP_WAIT != state || currStateCountDown > 0) //если не цикл поддержания температуры, или время не вышло, поддерживаем температуру...
      {
        if (_checkWarmUp(realValue, intValue, targetValue - TM_5C)) //если еще не нагрели, то пытаемся греть...
        {
          if (PCS_PREWARMUP_WAIT != state) //если температура нужная и не поддерживаем температуру, то начинаем поддерживать температуру заданное время
          {
            state = PCS_PREWARMUP_WAIT;
            currStateCountDown = TIM_PREWARMUP;
            tm1638::LEDOn(2);
            tm1638::LEDOn(3);
          }
        }
        break;
      }
      state = PCS_WARMUP_TO_TARGET;
      tm1638::LEDOn(3);
      tm1638::LEDOn(4);
    case PCS_WARMUP_TO_TARGET:
    case PCS_WAIT_FOR:
      if (PCS_WAIT_FOR != state || currStateCountDown > 0) //если не цикл поддержания температуры, или время не вышло, поддерживаем температуру...
      {
        if (_checkWarmUp(realValue, intValue, targetValue)) //если еще не нагрели, то пытаемся греть...
        {
          if (PCS_WAIT_FOR != state) //если температура нужная и не поддерживаем температуру, то начинаем поддерживать температуру заданное время
          {
            state = PCS_WAIT_FOR;
            currStateCountDown = targetTime;
            tm1638::LEDOn(4);
            tm1638::LEDOn(5);
          }
        }
        break;
      }
      //Отключаем нагрев и пищим об окончании...
      _powerOff();
      tm1638::LEDOn(5);
      tm1638::LEDOn(6);
      state = PCS_COOL_DOWN;
    case PCS_COOL_DOWN:
      if (currStateCountDown <= 0)
      {
        currStateCountDown = TIM_1Min;
        beeper::On(12800); // Включаем пищалку на 2 секунды
      }
      break;
  }

  gui::updateInfo(currTimer, realValue);
}

void powerControl::stop()
{
  if (!isStoped())
  {
    currStateCountDown = 0;
    state = PCS_STOPED;
    _powerOff();

    tm1638::allLEDOff();

    beeper::Off();
  }
  else if (currTimer > 0)
  {
    currTimer = 0;
  }
  else
  {
    targetValue = 0;
    targetTime = 0;
  }
}

void powerControl::sendStatToUART()
{
  usart::sendTemp(internalValue);
  usart::sendTemp(realValues[realValuePos]);
  usart::sendTemp(avgValue);
  usart::sendTemp(targetValue);
  usart::sendTime(currTimer);
  usart::trySend('\n');
}
