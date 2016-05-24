int led = 3;

void setup() {
  // put your setup code here, to run once:
//  Serial.begin(9600);
}

void loop() {
  for (int on = 0; on <= 255; on++)
  {
    analogWrite(led, on);
//    Serial.println(on);
    delay(10);
  }
//  delay(1000);
//  а вместо digitalWrite(led, LOW) вставить
  for (int off = 255; off >= 0; off--)
  {
    analogWrite(led, off);
//    Serial.println(off);
    delay(10);
  } 
//  delay(1000);
}
