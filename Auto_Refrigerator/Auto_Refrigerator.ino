#include <OneWire.h>
#include <DallasTemperature.h>

const int relePin = 13;
const int coolerPin = 12;
const int temperPin = 11;
const int voltageInputPin = 0;

int releState = LOW;
int _releState = LOW;
int collerState = HIGH;
int _collerState = HIGH;


float inTemp, outTemp;

float voltage = 12.0;
float _voltage = 12.0;

const float MAX_OUT_TEMPER = 55.0;
const float MIN_IN_TEMPER = 0.0;
const float LOW_VOLTAGE = 12.0;

const int DELAY_TIME = 5000;

OneWire  oneWire(temperPin);  // on pin 10 (a 4.7K resistor is necessary)
DallasTemperature sensors(&oneWire);

DeviceAddress inThermometer = {0x28, 0xFF, 0x2A, 0x1D, 0x93, 0x15, 0x01, 0x29 };
DeviceAddress outThermometer = {0x28, 0xFF, 0x2A, 0x1D, 0x93, 0x15, 0x01, 0x29 };

boolean waitForCold = false;
boolean waitForUpVoltage = true;

float R1 = 30000.0;//(30k+10k)/10k = 4 х 5V = max20V // (R1+R2)/R2
float R2 = 10000.0;

void setup() {
  pinMode(relePin, OUTPUT);
  pinMode(coolerPin, OUTPUT);
  pinMode(voltageInputPin, INPUT);

  sensors.begin();
  sensors.setResolution(inThermometer, 10);
  sensors.setResolution(outThermometer, 10);
}

void loop() {
  if (checkLowVoltage())
  {
    checkTempers();
  }

  delay(DELAY_TIME);
}

void checkTempers()
{
  if (checkInnerTemp && checkOutTemp)
  {
    _releState = HIGH;
  } else
    _releState = LOW;

  if (_releState != releState)
  {
    releState = _releState;
    digitalWrite(relePin, releState);
  }
}

boolean checkInnerTemp()
{
  inTemp = sensors.getTempC(inThermometer);

  if (inTemp <= MIN_IN_TEMPER)
    return false;
  else
    return true;
}

boolean checkOutTemp()
{
  outTemp = sensors.getTempC(outThermometer);

  if (waitForCold)
  {
    if (outTemp <= MAX_OUT_TEMPER - 10.0)
    {
      waitForCold = false;
      return true;
    } else
      return false;
  } else {
    if (outTemp >= MAX_OUT_TEMPER)
    {
      waitForCold = true;
      return false;
    }
    else
      return true;
  }
}

boolean checkLowVoltage()
{
  voltage = 13.0;
  float ACPU = analogRead(voltageInputPin);// код U АЦП (0-1024) 
  float vout = (ACPU * 5.0) / 1024;// напряжение (5В/1024)*АЦП
  voltage = vout / (R2 / (R1 + R2));

  if (waitForUpVoltage)
  {
    if (voltage >= LOW_VOLTAGE + 1.0)
    {
      waitForUpVoltage = false;
      digitalWrite(coolerPin, HIGH);
      return true;
    } else
      return false;
  } else
  {
    if (voltage <= LOW_VOLTAGE)
    {
      digitalWrite(relePin, LOW);
      digitalWrite(coolerPin, LOW);
      waitForUpVoltage = true;
      return false;
    }

    digitalWrite(coolerPin, HIGH);
    return true;
  }

}

