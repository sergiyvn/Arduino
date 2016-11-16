/*
  Modul nRF24 Send
  part of Arduino Mega Server project
*/

#ifdef NRF24_FEATURE

// temperature
#define NO_NEED_SEND          0
#define NEED_SEND_TEMP        1
#define NEED_SEND_ERROR       2
#define NEED_SEND_REMOVED     3
#define NEED_SEND_NOT_PRESENT 4
#define NEED_SEND_ALARM_UP    5
#define NEED_SEND_ALARM_DOWN  6
byte temp1NeedSend = NO_NEED_SEND;

// Make packet

void clearTxRegion(int start, int finish) {
  for (int i = start; i < finish; i++) {
    buffTx[i] = 0;
  }
}

void setTxRegion(int start, String str) {
  for (byte i = 0; i < str.length(); i++) {
    buffTx[start + i] = str[i];
  }
}

void makePacket(struct Sender *p, byte tpe, byte cmd, byte dest, String pwd, String dta) {
  // sender
  if (p->required) {p->active = true;  p->success = false;}
              else {p->active = false; p->success = true;}

  p->command = cmd;
  p->timer   = millis();
  
  byte type;
  if (p->type) {type = p->type;}
          else {type = tpe;}
  
  // packet
  buffTx[POS_TYPE]     = type;
  buffTx[POS_COMMAND]  = cmd;
  //////////////buffTx[POS_ADDRESS]  = eeAddress;
  buffTx[POS_DEST]     = dest;
  buffTx[POS_REQUIRED] = p->required;
  buffTx[POS_NUMBER]   = p->number;

  clearTxRegion(POS_PASSWORD, PACKET_MAX_BYTES);
  setTxRegion(POS_PASSWORD, pwd);
  setTxRegion(POS_DATA,     dta);
}

// Make packets

//void makePacketEcho()        {makePacket(&sendEcho,        TYPE_ECHO,    buffTx[POS_COMMAND], 255, "", "");}

void makePacketGetName()     {makePacket(&sendGetName,     TYPE_REQUEST, GET_NAME,      255, "", "");}
void makePacketGetAddress()  {makePacket(&sendGetAddress,  TYPE_REQUEST, GET_ADDR,      255, "", "");}
void makePacketGetFreeMem()  {makePacket(&sendGetFreeMem,  TYPE_REQUEST, GET_FREE_MEM,  255, "", "");}
void makePacketGetBattery()  {makePacket(&sendGetBattery,  TYPE_REQUEST, GET_BATTERY,   255, "", "");}
void makePacketGetSelfTemp() {makePacket(&sendGetSelfTemp, TYPE_REQUEST, GET_SELF_TEMP, 255, "", "");}
void makePacketGetHealth()   {makePacket(&sendGetHealth,   TYPE_REQUEST, GET_HEALTH,    255, "", "");}
void makePacketGetId()       {makePacket(&sendGetId,       TYPE_REQUEST, GET_ID,        255, "", "");}
void makePacketGetTemp1()    {makePacket(&sendGetTemp1,    TYPE_REQUEST, GET_TEMP_1,    255, "", "");}

// Wrappers

void sendMemoryAlarm()   {setPacketType(&sendGetFreeMem, TYPE_ALARM); sendPacket(DTA_FREE_MEM);}
void sendBatteryAlarm()  {setPacketType(&sendGetBattery, TYPE_ALARM); sendPacket(DTA_BATTERY);}
void sendSelfTempAlarm() {setPacketType(&sendGetSelfTemp,TYPE_ALARM); sendPacket(DTA_SELF_TEMP);}

// Send packet

void sendPacket(byte packet) {
  radio.stopListening();

  switch (packet) {
    case BAK_ECHO:       //makePacketEcho();
         break;
    case GET_NAME:       makePacketGetName();     break;
    case GET_ADDR:       makePacketGetAddress();  break;
    case GET_FREE_MEM:   makePacketGetFreeMem();  break;
    case GET_BATTERY:    makePacketGetBattery();  break;
    case GET_SELF_TEMP:  makePacketGetSelfTemp(); break;
    case GET_HEALTH:     makePacketGetHealth();   break;
    case GET_ID:         makePacketGetId();       break;
    case GET_TEMP_1:     makePacketGetTemp1();    break;
  } // switch (packet)

  #ifdef SERIAL_PRINT
    printPacketType(buffTx[POS_TYPE], OUT);
    printBuff(buffTx);
    Serial.println();
  #endif

  sendPackets++;

  if (!radio.write(&buffTx, PACKET_MAX_BYTES)) {
    #ifdef SERIAL_PRINT
      Serial.println(F("SEND FAILED"));
      sendErrors++;
    #endif
  }
  radio.startListening();
} // sendPacket( )

// Check retrys

void checkRetrys(struct Sender *p, byte command) {
  #define MAX_TRYOUTS  2
  #define TRYOUT_DELAY 3000

  if (p->active) {
    if (millis() - p->timer > TRYOUT_DELAY) {
      sendRetrys++;
      p->tryouts++;
      p->timer = millis();
      #ifdef SERIAL_PRINT
        Serial.print(F("TRY: "));
        Serial.println(p->tryouts);
      #endif
      sendPacket(command);
      if (p->tryouts > MAX_TRYOUTS) {
        sendErrors++;
        p->errors++;
        resetSender(p);
        p->success = false; // ! after resetSender(p)
        #ifdef SERIAL_PRINT
          Serial.print(F("SEND "));
          Serial.print(p->command);
          Serial.println(F(" ERROR"));
        #endif
      }
    }
  }
} // checkRetrys()

void retrysControl() {
  checkRetrys(&sendGetName,     GET_NAME);
  checkRetrys(&sendGetId,       GET_ID);
  checkRetrys(&sendGetAddress,  GET_ADDR);
  checkRetrys(&sendGetFreeMem,  GET_FREE_MEM);
  checkRetrys(&sendGetBattery,  GET_BATTERY);
  checkRetrys(&sendGetSelfTemp, GET_SELF_TEMP);
  checkRetrys(&sendGetHealth,   GET_HEALTH);
  #ifdef TEMP_FEATURE
    checkRetrys(&sendGetTemp1,  GET_TEMP_1);
  #endif
}

#endif // NRF24_FEATURE
