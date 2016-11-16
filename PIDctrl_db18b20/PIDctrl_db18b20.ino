#include <OneWire.h>
#include <DallasTemperature.h>

OneWire  oneWire(10);  // on pin 10 (a 4.7K resistor is necessary)

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

DeviceAddress roomThermometer = {0x28, 0xFF, 0x2A, 0x1D, 0x93, 0x15, 0x01, 0x29 };

// Параметры пропорционального звена
float kp = 1; // Коэффициент пропорционального звена

// Параметры интегратора
float Ki = 0.05; // Коэффициент интегрального звена
float ki;

#define iMin 0  // Минимальное значение интегратора
#define iMax 100  // Максимальное значение интегратора

float iSum = 0;  // Сумма ошибок (значение, накопленное в интеграторе)

// Параметры дифференциатора
float Kd = 0.0025;   // Коэффициент дифференциального звена
float kd;
float old_T = 0; // Предыдущее значение сигнала

float Input, Output;
float Setpoint = 60.0;

double startTime;

double SampleTime = 500;//ms

void setup() {
  Serial.begin(9600);

  double SampleTimeInSec = ((double)SampleTime) / 1000;
  ki = Ki * SampleTimeInSec;
  kd = Kd / SampleTimeInSec;

  sensors.begin();
  sensors.setResolution(roomThermometer, 10);
  startTime = millis();
}

void loop() {
  double currentTime = millis();
  if (currentTime - startTime >= SampleTime)
  {
    startTime = millis();

    sensors.requestTemperatures();
    Input = sensors.getTempC(roomThermometer);

    Serial.print("Temper in : ");
    Serial.println(Input);
    float val = Setpoint - Input;
    Output = PIDctl(val, Input);

    Serial.print("Temper out: ");
    Serial.println(Output);
  }

}


float PIDctl(float error, float y)
{
  float up, ui, ud;

  // Пропорциональная компонента
  up = kp * error;

  // Интегральная компонента
  iSum = iSum + error; // Накапливаем (суммируем)
  if (iSum < iMin) iSum = iMin; // Проверяем граничные значение
  if (iSum > iMax) iSum = iMax;
  ui = ki * iSum;

  // Дифференциальная компонента
  ud = kd * (y - old_T);
  old_T = y;

  return up + ui + ud;
}
