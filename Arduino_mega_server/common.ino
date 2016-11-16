/*
  Modul Common
  part of Arduino Mega Server project
  Generic functions
*/

/* --------------------------
  Function int freeMem()
    Return free RAM in Bytes
----------------------------- */

// Init

void started(char s[], boolean ts) {
  if (ts) {timeStamp();}
  Serialprint("Modul ");
  Serial.print(s);
  Serialprint("... started\n");
}

void initStart(char s[], boolean ts) {
  if (ts) {timeStamp();}
  Serialprint("Modul ");
  Serial.print(s);
  Serialprint("...\n");
}

void initDone(boolean ts) {
  if (ts) {timeStamp();}
  Serialprint("started\n");
}

// Print

void printValue(char s[], long n) {
  timeStamp();
  Serial.print(s);
  Serialprint(": ");
  Serial.println(n);  
}

// print IP

void printIp(byte ip[]) {
  for (byte i = 0; i < 4; i++) {
    Serial.print(ip[i]);
    if (i == 3) {break;}
    Serialprint(".");
  }
}

void printMac(byte mac[]) {
  for (byte i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i == 5) {break;}
    Serialprint(" ");
  }
}

// Send

void sendIpClient(byte ip[], EthernetClient cl) {
  for (byte i = 0; i < 4; i++) {
    cl.print(ip[i]);
    if (i == 3) {break;}
    cl.print(".");
  }
}

