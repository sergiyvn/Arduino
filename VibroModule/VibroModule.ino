#include <Bounce.h>

Bounce bouncerVibro = Bounce(10, 40); //создаем экземпляр класса Bounce для 10 вывода

void setup() {
  // put your setup code here, to run once:
 pinMode(9, OUTPUT); //Выход на пищалку
}

void loop() {
  if (bouncerVibro.update())
      //если произошло событие
      if (bouncerVibro.read() == 1) { 
        bouncerVibro.rebounce(500);    //повторить событие через 1000мс
        beep();
//        delay(1000);
      }
}

void beep() {
  //  Для пьезопещалки
  analogWrite(9, 20);      // значение должно находится между 0 и 255
  delay(50);          // пауза delayms мс
  analogWrite(9, 0);       // 0 - выключаем пьезо
  delay(50);          // пауза delayms мс

  //  Для динамиков
//  tone(beepPin, 950, delayms);
//  delay(2 * delayms);
}
