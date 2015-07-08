#include <Servo.h>

Servo servo;
int servoPin = 10;

void setup()
{
  servo.attach(servoPin);
  servo.write(180);
  delay(2000);
  servo.write(0);
  delay(250);
  servo.write(170);
}

void loop()
{
  //  servo.write(0);
  //  delay(200);
  //  servo.write(170);
  //  delay(2000);
}
