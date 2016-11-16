/*
  Modul Contacts
  part of Arduino Mega Server project
*/

#ifdef CONTACTS_FEATURE

#define cont1pin 27 // HOM
#define cont2pin 31 // CLO

#define OPEN  1
#define CLOSE 0

char cont1object[] = "contHOM";
char cont2object[] = "contCLO";

char cont1openMess[] = "Open"; char cont1closeMess[] = "Closed";
char cont2openMess[] = "Down"; char cont2closeMess[] = "Norm";

// states
  // 0 - open
  // 1 - close
  // 2 - not defined
  // 3 - not defined and not contXstatus
byte cont1state = 2; byte old_cont1state = 3; byte cont1objectState = 2;
byte cont2state = 2; byte old_cont2state = 3; byte cont2objectState = 2;

void contactsInit() {
  pinMode(cont1pin, INPUT);
  pinMode(cont2pin, INPUT);
  modulContacts = MODUL_ENABLE;
  started("Contacts", true);
}

void printAndSend(char obj[], char mess[]) {
  #ifdef EVENTS_CONTACTS
    timeStamp();
    Serial.print(obj);
    Serialprint(": ");
    Serial.println(mess);
  #endif
}

void contactsWorks() {
  // cont1 (HOM)
  cont1state = digitalRead(cont1pin);

  if (cont1state != old_cont1state) {
      old_cont1state = cont1state;
      
      if (cont1state == 0) {
        cont1objectState = OPEN;
        printAndSend(cont1object, cont1openMess);
        #ifdef MAJORDOMO_FEATURE
          sendRequestM(cont1object, OPEN);
        #endif
      } else {
        cont1objectState = CLOSE;
        printAndSend(cont1object, cont1closeMess);
        #ifdef MAJORDOMO_FEATURE
          sendRequestM(cont1object, CLOSE);
        #endif
      }
  }
  
  if (cycle1m) {
    old_cont1state = cont1state;

    if (cont1state == 0) {
      printAndSend(cont1object, cont1openMess);
      #ifdef MAJORDOMO_FEATURE
        sendRequestM(cont1object, 1);
      #endif
    } else {
        printAndSend(cont1object, cont1closeMess);
        #ifdef MAJORDOMO_FEATURE
          sendRequestM(cont1object, 0);
        #endif
      }
  }

  // cont2 (CLO)
  cont2state = digitalRead(cont2pin);
  
  if (cont2state == 1) {
    if (cont2state != old_cont2state) {
      old_cont2state = cont2state;
      printAndSend(cont2object, cont2openMess);
      #ifdef MAJORDOMO_FEATURE
        sendRequestM(cont2object, 1);
      #endif
    }
  } else {
      if (cont2state != old_cont2state) {
        old_cont2state = cont2state;
        printAndSend(cont2object, cont2closeMess);
        #ifdef MAJORDOMO_FEATURE
          sendRequestM(cont2object, 0);
        #endif
      }
  }

  if (cycle1m) {
    old_cont2state = cont2state;

    if (cont2state == 1) {
      printAndSend(cont2object, cont2openMess);
      #ifdef MAJORDOMO_FEATURE
        sendRequestM(cont2object, 1);
      #endif
    } else {
        printAndSend(cont2object, cont2closeMess);
        #ifdef MAJORDOMO_FEATURE
          sendRequestM(cont2object, 0);
        #endif
      }
  }
} // contactsWorks()

#endif // CONTACTS_FEATURE
