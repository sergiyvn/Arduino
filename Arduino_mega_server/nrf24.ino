/*
  Modul nRF24
  part of Arduino Mega Server project
*/

#ifdef NRF24_FEATURE

#include <SPI.h>
#include "RF24.h"

// nRF24L01 modul configuration
RF24 radio(8, 9); // CE, CSN pins
byte addresses[][6] = {"1Node", "2Node"};

// packet buffers
#define PACKET_MAX_BYTES 32
byte buffTx[PACKET_MAX_BYTES];
byte buffRx[PACKET_MAX_BYTES];

// packet structure
#define POS_TYPE      0
#define POS_COMMAND   1
#define POS_ADDR      2
#define POS_DEST      3
#define POS_REQUIRED  4
#define POS_NUMBER    5
#define POS_PASSWORD  9
#define POS_DATA      17

// packet type
#define TYPE_EMPTY   0
#define TYPE_COMMAND 1
#define TYPE_REQUEST 2
#define TYPE_DATA    3
#define TYPE_ALARM   4
#define TYPE_ANSWER  5
#define TYPE_ECHO    6

// packet variables
byte   rxType;
byte   rxCommand;
byte   rxAddr;
byte   rxDestination;
byte   rxRequired;
byte   rxNumber;
String rxPassword;
String rxData;

// required answer
#define NOT_REQUIRED 0
#define REQUIRED     1

// logic
#define NOT_DEF 0
#define OUT     0
#define IN      1


void nrf24Init() {
  initStart("nRF24", true);
  timeStamp();
  Serial.println(F(" Base Receiving"));
  radio.begin();
  radio.setPALevel(RF24_PA_LOW); // default RF24_PA_MAX
  
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
    
  radio.startListening();

  serialDynamic = ACTIVE;
  modulNrf24 = MODUL_ENABLE;
  initDone(true);
}

#ifdef SERIAL_PRINT
  void printBuff(byte buff[]) {
    for (byte i = 0; i < POS_PASSWORD; i++) {
      if      (buff[i] < 10)                 {Serial.print(F("   "));}
      else if (buff[i] > 9 && buff[i] < 100) {Serial.print(F("  "));}
      else if (buff[i] > 99)                 {Serial.print(F(" "));}
      Serial.print(buff[i]);
    }
    Serial.print(F(" "));
    for (byte i = POS_PASSWORD; i < POS_DATA; i++) {
      if (buff[i] == 0) {break;}
      Serial.print(char(buff[i]));
    }
    Serial.print(F(" "));
    for (byte i = POS_DATA; i < PACKET_MAX_BYTES; i++) {
      if (buff[i] == 0) {break;}
      Serial.print(char(buff[i]));
    }
  } // printBuff( )
#endif // SERIAL_PRINT

// set packet parameters

void setNotRequired(struct Sender *p) {
  p->required = NOT_REQUIRED;
}

void setPacketType(struct Sender *p, byte type) {
  p->type = type;
}

void setNextNumber(struct Sender *p) {
  p->number++;
  if (p->number > 255) {
    p->number = 0;
  }
}

boolean success(struct Sender *p) {
  return p->success;
}

// Make echo

void makeEcho() {
  for (byte i = 0; i < PACKET_MAX_BYTES; i++) {
    buffTx[i] = buffRx[i];
  }
  sendEcho.required = false;
  buffTx[POS_TYPE] = TYPE_ECHO;
}

// parse buffer

void parseBuffRx() {
  rxType        = buffRx[POS_TYPE];
  rxCommand     = buffRx[POS_COMMAND];
  rxAddr        = buffRx[POS_ADDR];
  rxDestination = buffRx[POS_DEST];
  rxRequired    = buffRx[POS_REQUIRED];
  rxNumber      = buffRx[POS_NUMBER];

  rxPassword = "";
  for (byte i = POS_PASSWORD; i < POS_DATA; i++) {
    if (buffRx[i] == 0) {break;}
    rxPassword += char(buffRx[i]);
  }
  rxData = "";
  for (byte i = POS_DATA; i < PACKET_MAX_BYTES; i++) {
    if (buffRx[i] == 0) {break;}
    rxData += char(buffRx[i]);
  }
}

#ifdef SERIAL_PRINT
  void printPacketType(byte type, byte dir) {
    String arrow;
    if (dir == IN) {arrow = F("<-");}
              else {arrow = F("->");}
  
    switch (type) {
      case TYPE_EMPTY:   Serial.print(F("Empty:   ")); Serial.print(arrow); break;
      case TYPE_COMMAND: Serial.print(F("Command: ")); Serial.print(arrow); break;
      case TYPE_REQUEST: Serial.print(F("Request: ")); Serial.print(arrow); break;
      case TYPE_DATA:    Serial.print(F("Data:    ")); Serial.print(arrow); break;
      case TYPE_ALARM:   Serial.print(F("Alarm:   ")); Serial.print(arrow); break;
      case TYPE_ANSWER:  Serial.print(F("Answer:  ")); Serial.print(arrow); break;
      case TYPE_ECHO:    Serial.print(F("Echo:    ")); Serial.print(arrow); break;
      default: Serial.print(F("Unknown: "));
    }
  }
#endif

void saveResult() {
  switch (rxCommand) {
    case DTA_NAME:      sendGetName.result     = rxData; break;
    case DTA_ID:        sendGetId.result       = rxData; break;
    case DTA_ADDR:      sendGetAddress.result  = rxData; break;
    case DTA_FREE_MEM:  sendGetFreeMem.result  = rxData; break;
    case DTA_BATTERY:   sendGetBattery.result  = rxData; break;
    case DTA_SELF_TEMP: sendGetSelfTemp.result = rxData; break;
    case DTA_HEALTH:    sendGetHealth.result   = rxData; break;
    case DTA_TEMP_1:    sendGetTemp1.result    = rxData; break;
  }
}

void nrf24Works() {
  if (radio.available()) {
    while (radio.available()) {
      radio.read(&buffRx, PACKET_MAX_BYTES);
    }
    
    receivePackets++;
    parseBuffRx();
    saveResult();

    #ifdef SERIAL_PRINT
      printPacketType(rxType, IN);
      printBuff(buffRx);
      Serial.println();
    #endif

    switch (rxType) {
      case TYPE_COMMAND: parseCommands(); break;
      case TYPE_DATA:    parseData();     break;
      case TYPE_ECHO:    parseEcho();     break;
      case TYPE_ANSWER:  parseAnswers();  break;
      case TYPE_REQUEST: parseRequests(); break;
      case TYPE_ALARM:   parseAlarms();   break;
      default: ;
    }
  } // if (radio.available())
  
  retrysControl();
} // nrf24Works()

// carousel
byte currPony = 0;

void carousel() {
  #define MAX_PONY 8
  currPony++;
  if (currPony > MAX_PONY) {currPony = 1;}
  switch (currPony) {
    case 1: if (nrf1Name     == "") {sendPacket(GET_NAME);}      break;
    case 2: if (nrf1Id       == "") {sendPacket(GET_ID);}        break;
    case 3: if (nrf1Addr     == 0)  {sendPacket(GET_ADDR);}      break;
    case 4: if (nrf1FreeMem  == 0)  {sendPacket(GET_FREE_MEM);}  break;
    case 5: if (nrf1Battery  == 0)  {sendPacket(GET_BATTERY);}   break;
    case 6: if (nrf1SelfTemp == 0)  {sendPacket(GET_SELF_TEMP);} break;
    case 7: if (nrf1Health   == 0)  {sendPacket(GET_HEALTH);}    break;
    case 8: if (nrf1Temp1    == 0)  {sendPacket(GET_TEMP_1);}    break;
  }
}

#endif // NRF24_FEATURE
