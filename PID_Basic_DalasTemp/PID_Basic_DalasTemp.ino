/********************************************************
 * PID Basic Example
 * Reading analog input 0 to control analog PWM output 3
 ********************************************************/

#include <PID_v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_INPUT 0
#define PIN_OUTPUT 3

OneWire  oneWire(10);  // on pin 10 (a 4.7K resistor is necessary)

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp = 1, Ki = 0.05, Kd = 0.25;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

DeviceAddress roomThermometer = {0x28, 0xFF, 0x2A, 0x1D, 0x93, 0x15, 0x01, 0x29 };

int SampleTime = 1000;

void setup()
{
  Serial.begin(9600);

  sensors.begin();
  sensors.setResolution(roomThermometer, 10);

  //initialize the variables we're linked to
  //  Input = analogRead(PIN_INPUT);
  Setpoint = 35.0;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 100);
  myPID.SetSampleTime(SampleTime);

}

void loop()
{
  //  Input = analogRead(PIN_INPUT);
  sensors.requestTemperatures();

  Input = sensors.getTempC(roomThermometer);
  Serial.print("Temper in : ");
  Serial.println(Input);
  myPID.Compute();
  Serial.print("Temper out: ");
  Serial.println(Output);
  //  analogWrite(PIN_OUTPUT, Output);
  delay(SampleTime);
}


