/*
  Modul PIRs
  part of Arduino Mega Server project
*/

#ifdef PIRS_FEATURE

// pins
#define pir1pin 22 // STD
#define pir2pin 23 // HAL
#define pir3pin 24 // KID
#define pir4pin 25 // KUH
#define pir5pin 26 // PRH
#define pir6pin 30 // CLO

// states
int pir1state = LOW; int pir1 = 0;
int pir2state = LOW; int pir2 = 0;
int pir3state = LOW; int pir3 = 0;
int pir4state = LOW; int pir4 = 0;
int pir5state = LOW; int pir5 = 0;
int pir6state = LOW; int pir6 = 0;

// init
void pirsInit() {
  pinMode(pir1pin, INPUT);
  pinMode(pir2pin, INPUT);
  pinMode(pir3pin, INPUT);
  pinMode(pir4pin, INPUT);
  pinMode(pir5pin, INPUT);
  pinMode(pir6pin, INPUT);
  modulPirs = MODUL_ENABLE;
  started("PIR's", true);
}

void detect(char s[]) {
  #ifdef EVENTS_PIRS
    timeStamp();
    Serialprint("Detect motion ");
    Serial.println(s);
  #endif
}

void endMmotion(char s[]) {
  #ifdef EVENTS_PIRS
    timeStamp();
    Serialprint("End motion ");
    Serial.println(s);
  #endif
}

// works
void pirsWorks() {
  // pir1 (STD)
  pir1 = digitalRead(pir1pin);
  if (pir1 == HIGH) { 
    if (pir1state == LOW) {
      detect("STD");
      pir1state = HIGH;
      #ifdef MAJORDOMO_FEATURE
        sendRequestM("pirSTD", 1);
      #endif
      #ifdef LEDS_FEATURE
        if (modeLED == LED_PIR) {
          //green(led1, bright);
        }
      #endif
    }
  } else {
    if (pir1state == HIGH){
      endMmotion("STD");
      pir1state = LOW;
      #ifdef MAJORDOMO_FEATURE
        sendRequestM("pirSTD", 0);
      #endif
      #ifdef LEDS_FEATURE
        //black(led1);
      #endif
    }
  }
    
  // pir2 (HAL)
  pir2 = digitalRead(pir2pin);
  if (pir2 == HIGH) { 
    if (pir2state == LOW) {
      detect("HAL");
      pir2state = HIGH;
      #ifdef MAJORDOMO_FEATURE
        sendRequestM("pirHAL", 1);
      #endif
      #ifdef LEDS_FEATURE
        if (modeLED == LED_PIR) {
          //blue(led1, bright);
        }
      #endif
    }
  } else {
    if (pir2state == HIGH){
      endMmotion("HAL");
      pir2state = LOW;
      #ifdef MAJORDOMO_FEATURE
        sendRequestM("pirHAL", 0);
      #endif
      #ifdef LEDS_FEATURE
        //black(led1);
      #endif
    }
  }

  // pir3 (KID)
  pir3 = digitalRead(pir3pin);
  if (pir3 == HIGH) { 
    if (pir3state == LOW) {
      // detect("KID");
      pir3state = HIGH;
      // setReqSens01("pirKID", 1);
      /*if (MODE == LED_PIR_01) {
        yellow(BRIGHT);
      }  */
    }
  } else {
    if (pir3state == HIGH){
      // endMmotion("KID");
      pir3state = LOW;
      // setReqSens01("pirKID", 0);
      #ifdef LEDS_FEATURE
        //black(led1);
      #endif
    }
  }

  // pir4 (KUH)
  pir4 = digitalRead(pir4pin);
  if (pir4 == HIGH) { 
    if (pir4state == LOW) {
      //detect("KUH");
      pir4state = HIGH;
      #ifdef MAJORDOMO_FEATURE
        //sendRequestM("pirKUH", 1);
      #endif
      #ifdef LEDS_FEATURE
        if (modeLED == LED_PIR) {
          //cyan(led1, bright);
        } 
      #endif
    }
  } else {
    if (pir4state == HIGH){
      //endMmotion("KUH");
      pir4state = LOW;
      #ifdef MAJORDOMO_FEATURE
        //sendRequestM("pirKUH", 0);
      #endif
      #ifdef LEDS_FEATURE
        //black(led1);
      #endif
    }
  }

  // pir5 (PRH)
  pir5 = digitalRead(pir5pin);
  if (pir5 == HIGH) { 
    if (pir5state == LOW) {
      detect("PRH");
      pir5state = HIGH;
      #ifdef MAJORDOMO_FEATURE
        sendRequestM("pirPRH", 1);
      #endif
      #ifdef LEDS_FEATURE
        if (modeLED == LED_PIR) {
          //red(led1, bright);
        }
      #endif
    }
  } else {
    if (pir5state == HIGH){
      endMmotion("PRH");
      pir5state = LOW;
      #ifdef MAJORDOMO_FEATURE
        sendRequestM("pirPRH", 0);
      #endif
      #ifdef LEDS_FEATURE
        //black(led1);
      #endif
    }
  }
  // pir6 (CLO)
  pir6 = digitalRead(pir6pin);
  if (pir6 == HIGH) { 
    if (pir6state == LOW) {
      detect("CLO");
      pir6state = HIGH;
      #ifdef MAJORDOMO_FEATURE
        sendRequestM("pirTLT", 1);
      #endif
      #ifdef LEDS_FEATURE
        if (modeLED == LED_PIR) {
          //magenta(led1, bright);
        } 
      #endif
    }
  } else {
    if (pir6state == HIGH){
      endMmotion("CLO");
      pir6state = LOW;
      #ifdef MAJORDOMO_FEATURE
        sendRequestM("pirTLT", 0);
      #endif
      #ifdef LEDS_FEATURE
        //black(led1);
      #endif
    }
  }
} // pirsWorks()

#endif // PIRS_FEATURE
