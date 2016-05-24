// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance

void setup()
{
  Serial.begin(9600);
  //calibration_value = ( i(measured) / i(sensor) ) / R(burden)
  //calibration_value = (141.4 A /  0.0707A) / 33Ω
  //calibration_value = 2000/33Ω = 60
  emon1.current(1, 55.6);             // Current: input pin, calibration.
}

void loop()
{
  double Irms = emon1.calcIrms(1480 * 3); // Calculate Irms only
  //  double val = analogRead(1);//digitalRead(1);
  //  Serial.print(val);
  //  Serial.print("    ");
  Serial.print(Irms * 230.0);	     // Apparent power
  Serial.print("    ");
  Serial.println(Irms);		       // Irms
}
