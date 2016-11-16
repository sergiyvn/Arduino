/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 */

/**
* ----------------------------------------------------------------------------
* This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
*
* NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
* ----------------------------------------------------------------------------
* This sample shows how to read and write data blocks on a MIFARE Classic PICC
* (= card/tag).
*
* BEWARE: Data will be written to the PICC, in sector #0 (blocks #1).
*
* Typical pin layout used:
* -----------------------------------------------------------------------------------------
*             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
*             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
* Signal      Pin          Pin           Pin       Pin        Pin              Pin
* -----------------------------------------------------------------------------------------
* RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
* SPI SS      SDA(SS)      10            53        D10        10               10
* SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
* SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
* SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*
*/

/***************************************************
  This is an example sketch for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 ****************************************************/

// For the breakout, you can use any (2 or) 3 pins
#define sclk 32
#define mosi 30
#define cs   28
#define dc   26
#define rst  24  // you can also connect this to the Arduino reset

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <MFRC522.h>
#include <EthernetUdp.h>

/* * * * * * * * * UDP Clock * * * * * * */

unsigned int localPort = 8888;       // local port to listen for UDP packets

char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

unsigned long currentTime = 0;
unsigned long syncTimeInt = 0;
unsigned long syncTimeMs = 0;
unsigned int syncTimeFrequensy = 3000000;//10mins in ms

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

/* * * * * * * * * TFT 1.8" * * * * * * */

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);


/* * * * * * * * * RDIF-RC522 * * * * * * */

// RDIF-RC522
#define RST_PIN         5           // Configurable, see typical pin layout above
#define SS_PIN          53          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

unsigned long uidDec, uidDecTemp;  // для храниения номера метки в десятичном формате

// PASSWORD in the KEY
byte keyCode[]    = {
  0xf0, 0xb4, 0x78, 0x3c,
  0xe1, 0xa5, 0x69, 0x2d,
  0xd2, 0x96, 0x5a, 0x1e,
  0xc3, 0x87, 0x4b, 0x0f
};

// KEYS UIDs in dec format // need to set manual
int keysUIDdec[]    = {
  429497114,
  2115029539
};

boolean securityActive = false;
boolean debugMode = false;
boolean silentMode = false;
boolean writeKeyCode = false;// do write keyCode into key

/* * * * * * * * * Ethernet W5100 shield microSD card * * * * * * */

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 4;

File myFile;

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;


/* * * * * * * * * Ethernet W5100 shield * * * * * * */

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEA
};

IPAddress ip(192, 168, 0, 200);
IPAddress myDns(192, 168, 0, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

String HTTP_req;          // stores the HTTP request
boolean LED_status = 0;   // state of LED, off by default

/* Per pins - motion sensors*/
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 1;//30;

//the time when the sensor outputs a low impulse
unsigned int lowIn;

//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
unsigned int pause = 5000;

boolean lockLow = true;
boolean takeLowTime;

boolean motionDetected = false;
unsigned long motionDetectedTime;
unsigned long motionEndedTime;
unsigned long motionDetectedTimeMS;
unsigned long motionEndedTimeMS;
int pir1Pin = 23;

/* * * * * * * * * Others * * * * * * */

int pinBeep = 9;
int pinPowerLed = 22;

const int RED_PIN = 36;
const int GREEN_PIN = 38;
const int BLUE_PIN = 40;
int DISPLAY_TIME = 10;  // In milliseconds

// COLORS
const int OFF = 0;
const int RED = 1;
const int GREEN = 2;
const int BLUE = 3;
const int YELLOW = 4;
const int AQUA = 5;
const int WHITE = 6;

boolean tftDataChanged = true;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // If your TFT's plastic wrap has a Black Tab, use the following:
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // If your TFT's plastic wrap has a Red Tab, use the following:
  //tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  //tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab

  tft.fillScreen(ST7735_BLACK);

  // init RDIF-RC522
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  mfrc522.PCD_DumpVersionToSerial();

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }


  /*Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  }
  else {
    Serial.println("example.txt doesn't exist.");
    Serial.println("Creating example.txt...");
    myFile = SD.open("example.txt", FILE_WRITE);
    myFile.close();
  }


  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);

  */

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  if (debugMode) {
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
  }

  Udp.begin(localPort);

  pinMode(9, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  /* Per pins - motion sensors */
  pinMode(pir1Pin, INPUT);
  digitalWrite(pir1Pin, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    //    Serial.print(".");
    Serial.println(calibrationTime - i);
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);

  beep_and_blink(3, 150);

  tft.fillRect(0, 0 , tft.width(), 20, ST7735_BLACK);
  drawText("Security:", ST7735_WHITE, 5, 5, 1);
  drawText("Motion detected:", ST7735_WHITE, 5, 15, 1);
}




/*
*
* * * * * * * * * * LOOP() * * * * * * * * * * *
*
*/


void loop() {
  /*  Display data on Screen */
  if (tftDataChanged)
  {
    tftDataChanged = false;

    printSecurityState();
    printMotionState();
  }

  currentTime = millis();
//  if (currentTime >= 120000)
//    software_Reset();

  if ( syncTimeMs == 0 || syncTimeInt == 0 || currentTime - syncTimeMs >= syncTimeFrequensy )
    getClock();

  /*
  *
  * * * * MOTION DETECT functionality * * * *
  *
  */

  if (digitalRead(pir1Pin) == HIGH) {
    motionDetected = true;

    //    digitalWrite(pinBeep, HIGH);   //the led visualizes the sensors output pin state
    digitalWrite(pinPowerLed, HIGH);  //the led visualizes the sensors output pin state

    if (lockLow) {
      tftDataChanged = true;
      printMotionState();

      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;

      motionDetectedTimeMS = currentTime;
      motionEndedTime = 0;

      unsigned long tm1 = motionDetectedTimeMS - syncTimeMs;
      motionDetectedTime = syncTimeInt + tm1 / 1000;

      if (debugMode) {
        Serial.println("---");
        Serial.print("motion detected at ");
        Serial.print(convertTime(motionDetectedTime));
        Serial.println(" sec");
      }
      delay(50);
    }

    takeLowTime = true;
  }

  if (digitalRead(pir1Pin) == LOW) {

    //    digitalWrite(pinBeep, LOW);  //the led visualizes the sensors output pin state
    digitalWrite(pinPowerLed, LOW);  //the led visualizes the sensors output pin state

    if (takeLowTime) {
      lowIn = currentTime;          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause,
    //we assume that no more motion is going to happen
    if (!lockLow && currentTime - lowIn > pause) {
      motionDetected = false;
      tftDataChanged = true;
      printMotionState();

      //makes sure this block of code is only executed again after
      //a new motion sequence has been detected
      lockLow = true;

      motionEndedTimeMS = (currentTime - pause);

      unsigned long tm2 = motionEndedTimeMS - syncTimeMs;
      motionEndedTime = syncTimeInt + tm2 / 1000;

      if (debugMode) {
        Serial.print("motion ended at ");      //output
        Serial.print(convertTime(motionEndedTime));
        Serial.println(" sec");
      }
      delay(50);
    }
  }


  /*
  *
  * * * * SERVER functionality * * * *
  *
  */

  // listen for incoming clients
  EthernetClient client = server.available();
  //  Serial.print("client: ");
  //  Serial.println(client);
  if (client) {
    if (debugMode)
      Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //        Serial.write(c);//Print client params in console

        HTTP_req += c;  // save the HTTP request 1 char at a time

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<body style=\"background-color:powderblue;\">");
          // output the value of each analog input pin
          /*for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<a href=\"/$1\"><button>On</button></a>");
            client.println("<a href=\"/$2\"><button>Off</button></a>");
            client.println("<br />");
          }*/

          client.print("Current time: ");
          client.print("<b>");
          client.print(convertTime(syncTimeInt + ((currentTime - syncTimeMs) / 1000));
          client.println("</b><br/>");

          client.print("Security: ");
          if (securityActive) {
            //            client.println("ON");
            client.println("<b style=\"color:blue; fontWeight:bold\">ON</b>");
          } else {
            //            client.println("OFF");
            client.println("<b style=\"color:green;\">OFF</b>");
          }
          client.println("<br/>");

          client.print("Motion detected: ");
          if (motionDetected)
          {
            client.println("<b style=\"color:red;\">YES</b>");
            //            client.println("YES");
          } else {
            client.println("<b style=\"color:green;\">NO</b>");
            //            client.println("No");
          }
          client.println("<br/>");

          client.print("motion detected at: ");
          client.println(convertTime(motionDetectedTime));
          client.println(" sec");
          client.println("<br/>");

          client.print("motion ended at: ");
          client.println(convertTime(motionEndedTime));
          client.println(" sec");
          client.println("<br/>");

          //          client.println("<form method=\"get\">");
          //          ProcessCheckbox(client);
          //          client.println("</form>");

          client.println("</body>");
          client.println("</html>");
          Serial.print("currentTime loop: ");
          Serial.println(currentTime);
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }

    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    if (debugMode)
      Serial.println("client disconnected");
  }



  /*
  *
  * * * * SECURITY functionality * * * *
  *
  */

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    delay(500);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    if (debugMode) {
      Serial.println(F("cannot select card:"));
    }
    delay(500);
    return;
  }

  uidDec = 0;
  // Выдача серийного номера метки.
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec * 256 + uidDecTemp;
  }

  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

  if (debugMode) {
    Serial.println("Card UID: ");
    Serial.println(uidDec); // Выводим UID метки в консоль.
    Serial.print(F("PICC type: "));
    Serial.println(mfrc522.PICC_GetTypeName(piccType));
  }

  // Check for compatibility
  if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    if (debugMode)
      Serial.println(F("This sample only works with MIFARE Classic cards."));
    tone_and_blink(1, 1000);
    return;
  }

  // In this sample we use the second sector,
  // that is: sector #0, covering block #1
  byte sector         = 0;
  byte blockAddr      = 1;
  byte trailerBlock   = 0;
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Authenticate using key A
  if (debugMode)
    Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    if (debugMode) {
      Serial.print(F("PCD_Authenticate() with key A failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
    }
    tone_and_blink(1, 1000);
    return;
  }

  // Read data from the block
  if (debugMode) {
    Serial.print(F("Reading data from block "));
    Serial.print(blockAddr);
    Serial.println(F(" ..."));
  }
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  if (debugMode) {
    Serial.print(F("Data in block "));
    Serial.print(blockAddr);
    Serial.println(F(":"));
    dump_byte_array(buffer, 16);
    Serial.println();
  }

  // Authenticate using key B
  if (debugMode)
    Serial.println(F("Authenticating again using key B..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    if (debugMode) {
      Serial.print(F("PCD_Authenticate() with key B failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
    }
    tone_and_blink(1, 1000);
    return;
  }

  // Write data to the block
  if (writeKeyCode) {
    if (debugMode) {
      Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
      Serial.println(F(" ..."));
      dump_byte_array(keyCode, 16); Serial.println();
    }
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, keyCode, 16);
    if (debugMode) {
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
      }
      Serial.println();
    }
  }

  // Check that data in block is what we have written
  // by counting the number of bytes that are equal
  if (debugMode)
    Serial.println(F("Checking result..."));

  boolean dataMatch = false;
  byte count = 0;
  for (byte i = 0; i < 16; i++) {
    // Compare buffer (= what we've read) with keyCode (= what we've written)
    if (buffer[i] == keyCode[i])
      count++;
  }
  if (debugMode)
    Serial.print(F("Number of bytes that match = ")); Serial.println(count);

  if (count == 16) {
    if (debugMode)
      Serial.println(F("Success :-)"));
    dataMatch = true;
  } else {
    if (debugMode)
      Serial.println(F("Failure, no match :-("));
  }

  // Dump the sector data
  if (debugMode) {
    Serial.println();
    Serial.println(F("Current data in sector:"));
  }
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  if (debugMode)
    Serial.println();

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();

  if (dataMatch == true) // Сравниваем Uid метки, если он равен заданому то серва открывает.
  {
    boolean keyValid = false;
    for (int i = 0; i < sizeof(keysUIDdec); i++)
    {
      if (uidDec == keysUIDdec[i])
      {
        keyValid = true;
        break;
      }
    }

    if (keyValid)
    {
      if (securityActive)
      {
        showColor(GREEN);

        securityActive = false;
        tftDataChanged = true;
        printSecurityState();

        //        tone_and_blink(2, 500);
        beep_and_blink(2, 100);
        if (debugMode)
          Serial.println(F("Security deactivated."));
        delay(5000);
        showColor(OFF);
      } else {
        showColor(YELLOW);

        securityActive = true;
        tftDataChanged = true;
        printSecurityState();

        //        tone_and_blink(1, 500);
        beep_and_blink(1, 300);
        if (debugMode)
          Serial.println(F("Security activated."));
        delay(5000);
        showColor(OFF);
      }
    } else
      tone_and_blink(1, 1000);
  } else
    tone_and_blink(1, 1000);

  delay(1000);
}

void beep_and_blink(int times, int del)
{
  for (int i = 0; i < times; i++)
  {
    if (!silentMode)
      digitalWrite(pinBeep, HIGH);
    digitalWrite(pinPowerLed, HIGH);
    delay(del);
    if (!silentMode)
      digitalWrite(pinBeep, LOW);
    digitalWrite(pinPowerLed, LOW);
    delay(del);
  }
}

void tone_and_blink(int times, int del)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(pinPowerLed, HIGH);
    if (!silentMode)
      tone(9, 200, del);//pin, frequency, duration
    delay(del);
    digitalWrite(pinPowerLed, LOW);
    delay(del);
  }
}

void ProcessCheckbox(EthernetClient cl)
{
  if (HTTP_req.indexOf("LED2=2") > -1) {  // see if checkbox was clicked
    // the checkbox was clicked, toggle the LED
    if (LED_status) {
      LED_status = 0;
    }
    else {
      LED_status = 1;
    }
  }

  if (LED_status) {    // switch LED on
    digitalWrite(22, HIGH);
    // checkbox is checked
    cl.println("<input type=\"checkbox\" name=\"LED2\" value=\"2\" \
        onclick=\"submit();\" checked>LED2");
  }
  else {              // switch LED off
    digitalWrite(22, LOW);
    // checkbox is unchecked
    cl.println("<input type=\"checkbox\" name=\"LED2\" value=\"2\" \
        onclick=\"submit();\">LED2");
  }
}

/**
* Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void showColor(int color)
{
  switch (color)
  {
    case RED:
      setColor(255, 0, 0);  // red
      break;
    case GREEN:
      setColor(0, 255, 0);  // green
      break;
    case BLUE:
      setColor(0, 0, 255);  // blue
      break;
    case YELLOW:
      setColor(255, 255, 0);// yellow
      break;
    case AQUA:
      setColor(0, 255, 255);  // aqua
      break;
    case WHITE:
      setColor(0, 255, 255);// white
      break;
    default:
      setColor(0, 0, 0);// off
      break;
  }
}

void setColor(int red, int green, int blue)
{
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void drawText(char* value, uint16_t color, int x, int y, int sizeT)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(sizeT);
  tft.print(value);
}


void printSecurityState()
{
  //  drawText("Security:", ST7735_WHITE, 5, 5, 1);
  tft.fillRect(65, 5, 30, 10, ST7735_BLACK);
  if (securityActive)
  {
    drawText("ON", ST7735_YELLOW, 65, 5, 1);
  } else {
    drawText("OFF", ST7735_GREEN, 65, 5, 1);
  }
}

void printMotionState()
{
  //  drawText("Motion detected:", ST7735_WHITE, 5, 15, 1);
  tft.fillRect(105, 15, 20, 10, ST7735_BLACK);
  if (motionDetected)
  {
    drawText("YES", ST7735_RED, 105, 15, 1);
  } else {
    drawText("NO", ST7735_GREEN, 105, 15, 1);
  }
}


void getClock() {
  unsigned long syncTime = millis();

  sendNTPpacket(timeServer); // send an NTP packet to a time server

  // wait to see if a reply is available
  delay(1000);
  if (Udp.parsePacket()) {
    syncTimeMs = syncTime;

    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.print(epoch);
    Serial.print(", ");
    epoch += 3600L * 3;
    Serial.println(epoch);

    syncTimeInt = 0;
    syncTimeInt = epoch;
    Serial.print("Time: ");
    Serial.println(convertTime(syncTimeInt));
    Serial.println(convertTime(epoch));
  }

  // wait ten seconds before asking for the time again
  Ethernet.maintain();
}

String convertTime(unsigned long value)
{
  Serial.print("convertTime: ");
  Serial.println(value);
  String tm = "";
  unsigned long epoch = (value % 86400);
  Serial.println(epoch);
  tm += String(epoch / 3600);
  Serial.println(tm);
  tm += ':';
  Serial.println(tm);
  if (((epoch % 3600) / 60) < 10) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    tm += '0';
    Serial.println(tm);
  }
  tm += String((epoch  % 3600) / 60);
  Serial.println(tm);
  tm += ':';
  Serial.println(tm);
  if ((epoch % 60) < 10) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    tm += '0';
    Serial.println(tm);
  }
  tm += String(epoch % 60);

  Serial.print("tm: ");
  Serial.println(tm);
  return tm;
}

// send an NTP request to the time server at the given address
void sendNTPpacket(char* address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
  asm volatile ("  jmp 0");
}
