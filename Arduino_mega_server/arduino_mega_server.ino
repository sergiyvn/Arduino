/*
  Arduino Mega Server
  version 0.16
  2016, Hi-Lab.ru
  
  License: Free, without any warranties
  Home:    http://hi-lab.ru/arduino-mega-server (russian)
           http://hi-lab.ru/english/arduino-mega-server (english)
  Email:   info@hi-lab.ru
  
  IDE: Arduino 1.6.5 r2
       Processing 1.5.1

  Sketch settings:
  ----------------
  
  (on this page)
  
  W5100 (default)
  #include <Ethernet.h>
  #include <EthernetUdp.h>
  //#include <Ethernet2.h>
  //#include <EthernetUdp2.h>

  W5500 (if Ethernet Shield 2 on W5500 chip)
  //#include <Ethernet.h>
  //#include <EthernetUdp.h>
  #include <Ethernet2.h>
  #include <EthernetUdp2.h>
  
  1. authMode = AUTH_OFF (default)
                AUTH_ON (required authorization login/password (default - admin:ams)
  2. timeProvider = TIME_NETWORK (default)
                    TIME_RTC (hardware RTC)
                    TIME_NONE (no time synchronization)
  3. freeSockets = FREE_SOCKETS_ONE (default)
                   FREE_SOCKETS_ZERO (no network functions and fast interface)
  4. //#define MAJORDOMO_FEATURE (default)
       #define MAJORDOMO_FEATURE (send data and synchronize time with MajorDoMo)

  (on Ethernet page)
  
  SELF_IP[] = {192, 168, 1, 37}; (default)
  GATEWAY[] = {192, 168, 1,  1}; (default)

  Pathes of project AMS:
  ------------------------------
  \Sketches\ams\Arduino\arduino_mega_server\
  \Sketches\ams\Arduino\libraries\
  \Sketches\ams\Processing\arduino_serial_commander\

  Arduino IDE settings:
  ---------------------
  Sketches folder:
  \Sketches\ams\Arduino

  Loading sketches:
  ------------------  
  Arduino Mega Server:      arduino_mega_server.ino
  Arduino Serial Commander: arduino_serial_commander.pde
  
  Quick start:
  ------------
  1. Files from archive microSD.zip - to microSD card (files to root!)
  2. Sketch "arduino_mega_server.ino" - to Arduino Mega
  3. Open on your browser address "192.168.1.37"
  4. Enjoy and donate on page
     http://hi-lab.ru/arduino-mega-server/details/donate
*/

// modules
#define ETHERNET_FEATURE
#define SD_INFO_FEATURE
//#define SD_FILES_FEATURE
#define SERVER_FEATURE
#define RTC_FEATURE
#define NTP_FEATURE
#define SEND_FEATURE
//#define MAJORDOMO_FEATURE
#define UPLOAD_FEATURE
//#define PING_FEATURE
//#define LAURENT_FEATURE
//#define PIRS_FEATURE
//#define CONTACTS_FEATURE
#define TEMP_FEATURE
//#define ELECTRO_FEATURE
#define LEDS_FEATURE
//#define KEYS_FEATURE
#define NOO_FEATURE
//#define MR1132_FEATURE
//#define NRF24_FEATURE

// debug
//#define ELECTRO_DEBUG
//#define SERVER_PROFILING
//#define EVENTS_CONTACTS
//#define EVENTS_PIRS
//#define TEMP_DEBUG
#define SERIAL_PRINT

#include <avr/pgmspace.h>
#include <SPI.h>

// W5100
#include <Ethernet.h>
#include <EthernetUdp.h>

// W5500
//#include <Ethernet2.h>
//#include <EthernetUdp2.h>

#include <SD.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

char const SELF_NAME[] = "AMS Mega";
char const VERSION[]   = "0.16";

// Authorization
#define AUTH_OFF 0
#define AUTH_ON  1
byte authMode = AUTH_OFF;
// Online encode to Base64: base64encode.org
String AUTH_HASH = "Authorization: Basic YWRtaW46YW1z"; // admin:ams

#define PASSIVE 0
#define ACTIVE  1
byte serialDynamic = PASSIVE;

// time provider
#define TIME_NONE    0
#define TIME_NETWORK 1
#define TIME_RTC     2
byte timeProvider = TIME_NETWORK;

// free sockets
#define FREE_SOCKETS_ZERO 0
#define FREE_SOCKETS_ONE  1
byte freeSockets = FREE_SOCKETS_ONE;

// mode work
#define MODE_SERVER 1
#define MODE_UPDATE 2
byte modeWork = MODE_SERVER;

// duration
time_t prevEventElectroCycle;
time_t prevEventMegaLive;
time_t prevEventRequest;

#ifdef NRF24_FEATURE
  // commands
  #define GET_NAME      1
  #define DTA_NAME      2
  #define SET_NAME      3
  #define GET_ADDR      4
  #define DTA_ADDR      5
  #define SET_ADDR      6
  #define GET_FREE_MEM  7
  #define DTA_FREE_MEM  8
  #define GET_BATTERY   9
  #define DTA_BATTERY   10
  #define GET_SELF_TEMP 11
  #define DTA_SELF_TEMP 12
  #define GET_TEMP_1    13
  #define DTA_TEMP_1    14
  #define GET_HEALTH    15
  #define DTA_HEALTH    16
  #define GET_ID        100
  #define DTA_ID        101
  #define SET_ID        102
  #define GEN_ID        103
  #define BAK_ECHO      200

  // sender structure
  typedef struct Sender {
    boolean active      = false;
    boolean required    = true;
    boolean success     = false;
    byte type           = 0;
    byte command        = 0;
    byte number         = 0;
    byte tryouts        = 0;
    int errors          = 0;
    String result       = "";
    unsigned long timer = millis();
  } sender;

  sender sendEcho,
         sendGetName, sendGetId, sendGetAddress,
         sendGetFreeMem, sendGetBattery, sendGetSelfTemp,
         sendGetHealth,
         sendGetTemp1;

 // net stat
 int sendPackets        = 0;
 int sendRetrys         = 0;
 int sendErrors         = 0;
 int receivePackets     = 0;
 int successPackets     = 0;
 int sendPacketsStat    = 0;
 int sendRetrysStat     = 0;
 int sendErrorsStat     = 0;
 int receivePacketsStat = 0;
 int successPacketsStat = 0;
    
#endif // NRF24_FEATURE

// nRF24 controller
  String nrf1Name;
  String nrf1Id;
  byte   nrf1Addr;
  byte   nrf1Battery;
  byte   nrf1Health;
  int    nrf1FreeMem;
  float  nrf1SelfTemp;
  float  nrf1Temp1;

// modules
#define MODUL_DISABLE       0
#define MODUL_ENABLE        1
#define MODUL_NOT_COMPILLED 2
byte modulEthernet = MODUL_NOT_COMPILLED;
byte modulRtc      = MODUL_NOT_COMPILLED;
byte modulNtp      = MODUL_NOT_COMPILLED;
byte modulSdCard   = MODUL_NOT_COMPILLED;
byte modulServer   = MODUL_NOT_COMPILLED;
byte modulSend     = MODUL_NOT_COMPILLED;
byte modulMajor    = MODUL_NOT_COMPILLED;
byte modulLaurent  = MODUL_NOT_COMPILLED;
byte modulPing     = MODUL_NOT_COMPILLED;
byte modulUpload   = MODUL_NOT_COMPILLED;
byte modulPirs     = MODUL_NOT_COMPILLED;
byte modulContacts = MODUL_NOT_COMPILLED;
byte modulTemp     = MODUL_NOT_COMPILLED;
byte modulElectro  = MODUL_NOT_COMPILLED;
byte modulLeds     = MODUL_NOT_COMPILLED; 
byte modulKeys     = MODUL_NOT_COMPILLED;
byte modulNoo      = MODUL_NOT_COMPILLED;
byte modulMr1132   = MODUL_NOT_COMPILLED;
byte modulNrf24    = MODUL_NOT_COMPILLED;

// strings
char buf[200];

// for sample Ajax
boolean LED_state[4] = {0};

// electro
boolean buttonElectro = 0;

// oscill
boolean oscill = false;

// cyclos
unsigned long cyclos = 0;

// timers
unsigned long timeSec; // time in seconds
unsigned long timer1s;  boolean cycle1s  = false;
unsigned long timer4s;  boolean cycle4s  = false;
unsigned long timer20s; boolean cycle20s = false;
unsigned long timer30s; boolean cycle30s = false;
unsigned long timer1m;  boolean cycle1m  = false;
unsigned long timer3m;  boolean cycle3m  = false;
unsigned long timer5m;  boolean cycle5m  = false;
int startSendTime = 10;

#ifdef LEDS_FEATURE
  // LED modes
  #define LED_EMPTY 0
  #define LED_PIR   1
  byte modeLED = LED_EMPTY;
#endif

// Serialprint
void StreamPrint_progmem(Print &out,PGM_P format,...) {
  // program memory version of printf - copy of format string and result share a buffer so as to avoid too much memory use
  char formatString[128], *ptr;
  strncpy_P(formatString, format, sizeof(formatString)); // copy in from program mem
  // null terminate - leave last char since we might need it in worst case for result's \0
  formatString[sizeof(formatString) - 2] = '\0'; 
  ptr = &formatString[strlen(formatString) + 1]; // our result buffer...
  va_list args;
  va_start(args, format);
  vsnprintf(ptr, sizeof(formatString) - 1 - strlen(formatString), formatString, args);
  va_end(args);
  formatString[sizeof(formatString) - 1] = '\0'; 
  out.print(ptr);
}

#define Serialprint(format, ...) StreamPrint_progmem(Serial,PSTR(format),##__VA_ARGS__)
#define Streamprint(stream,format, ...) StreamPrint_progmem(stream,PSTR(format),##__VA_ARGS__)

/* setup
-------------------------------------------------- */

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print(SELF_NAME);
  Serial.print(" (");
  Serial.print(VERSION);
  Serial.println(F(") started..."));
  hardwareInit();
  randomInit();
  eepromInit();

  #ifdef ETHERNET_FEATURE
    ethernetInit();
  #endif

  sdCardInit();
  #ifdef SD_INFO_FEATURE
    sdInfoInit();
  #endif
  #ifdef SD_FILES_FEATURE
    sdFilesInit();
  #endif
  #ifdef NTP_FEATURE
    ntpInit();
  #endif
  #ifdef RTC_FEATURE
    rtcInit();
  #endif
  #ifdef SERVER_FEATURE
    serverInit();
  #endif

  timersInit();
  
  #ifdef MAJORDOMO_FEATURE
    majordomoInit();
    majordomoMegaLive();
  #endif
  #ifdef LAURENT_FEATURE
    laurentInit();
  #endif
  #ifdef PING_FEATURE
    pingInit();
  #endif
  #ifdef UPLOAD_FEATURE
    uploadInit();
  #endif
  #ifdef PIRS_FEATURE
    pirsInit();
  #endif
  #ifdef CONTACTS_FEATURE
    contactsInit();
  #endif
  #ifdef TEMP_FEATURE
    tempInit();
  #endif
  #ifdef ELECTRO_FEATURE
    electroInit();
  #endif
  #ifdef KEYS_FEATURE
    keysInit();
  #endif
  #ifdef LEDS_FEATURE
    ledsInit();
  #endif
  #ifdef NOO_FEATURE
    nooInit();
  #endif
  #ifdef MR1132_FEATURE
    mr1132Init();
  #endif
  #ifdef NRF24_FEATURE
    nrf24Init();
  #endif

  timeStamp();
  Serialprint("GLOBAL Init DONE (");
  Serial.print(millis() / 1000);
  Serial.println(F("s)\n"));
  timeStamp();
  Serialprint("AMS WORK "); printFreeMem(); Serialprint("\n");
} // setup

/* loop
-------------------------------------------------- */

void loop() {
  
  #ifdef UPLOAD_FEATURE
    uploadWorks();
  #endif

  if (modeWork == MODE_SERVER) {
    profStart();
    timersWorks();
    #ifdef RTC_FEATURE
      rtcWorks();
    #endif
    #ifdef SERVER_FEATURE
      serverWorks();
    #endif
    #ifdef KEYS_FEATURE
      keysWorks();
    #endif
    #ifdef PIRS_FEATURE
      pirsWorks();
    #endif
    #ifdef CONTACTS_FEATURE
      contactsWorks();
    #endif
    #ifdef TEMP_FEATURE
      if (cycle5m) {
        tempWorks();
      }
    #endif 
    #ifdef ELECTRO_FEATURE
      if (modulElectro == 1) {
        if (cycle20s || (timeSec < startSendTime)) {
          electroWorks();
          freqWorks();
        }
        // network oscilloscope
        if (cycle1s && oscill) {
          freqWorks();
        }
      }
    #endif
    #ifdef MAJORDOMO_FEATURE
      majordomoMegaLive();
    #endif
    #ifdef MR1132_FEATURE
      mr1132Work();
    #endif
    #ifdef PING_FEATURE
      if (cycle30s) {
        pingWorks();
      }
    #endif
    #ifdef LAURENT_FEATURE
      if (cycle3m) {
        sprintf(buf, "$KE");
        sendLaurentRequest();
      }
    #endif
    #ifdef NRF24_FEATURE
      nrf24Works();

      if (timer1(1000, 9000)) {
        carousel();
      }
    #endif
    if (cycle1m) {
      timeStamp();
      printFreeMem();
      Serialprint("\n");
    }
  
    cyclos++;
    cyclosInSecWork();
    
    // end loop works
    if (serialDynamic) {print1s();}
    eraseCyclos();
    profCalc();

    cyclosDelayWork();

  } // if (modeWork == MODE_SERVER)
} // loop

