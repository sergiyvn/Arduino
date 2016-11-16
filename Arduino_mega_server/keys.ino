/*
  Modul Keys
  part of Arduino Mega Server project
*/

#ifdef KEYS_FEATURE

int goLight = 0; // Light (key) control
#define keyPin 3

void keysInit() {
  pinMode(keyPin, OUTPUT);
  modulKeys = MODUL_ENABLE;
  started("Keys", true);
}

void keysWorks() {
  if (goLight == 1) {
    digitalWrite(keyPin, LOW);
  } else {
      digitalWrite(keyPin, HIGH);   
    }
}

#endif // KEYS_FEATURE
