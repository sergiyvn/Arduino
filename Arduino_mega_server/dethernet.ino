/*
  Modul Ethernet
  part of Arduino Mega Server project
*/

// Ethernet settings
byte SELF_MAC[] = {0x00, 0x2A, 0xF5, 0x12, 0x67, 0xFA};
byte SELF_IP[] =  {192, 168, 1, 37};
//byte DNS[] = { , , , };
byte GATEWAY[] = {192, 168, 1, 1};

void ethernetInit() {
  Serialprint("Modul Ethernet...\n");
  Ethernet.begin(SELF_MAC, SELF_IP, GATEWAY);
  Serialprint(" MAC: ");
  printMac(SELF_MAC);
  Serial.println();
  Serialprint(" IP: ");
  printIp(SELF_IP);
  Serial.println();
  modulEthernet = MODUL_ENABLE;
  Serialprint("started\n");
}

void enableEthernet()  {pinMode(10, OUTPUT); digitalWrite(10, LOW);}
void disableEthernet() {pinMode(10, OUTPUT); digitalWrite(10, HIGH);}
void enableSdCard()    {pinMode( 4, OUTPUT); digitalWrite( 4, LOW);}
void disableSdCard()   {pinMode( 4, OUTPUT); digitalWrite( 4, HIGH);}

