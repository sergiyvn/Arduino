/*
  Modul Server Ajax
  part of Arduino Mega Server project
*/

#ifdef SERVER_FEATURE

#define DEFAULT_DESIGN 0
#define HOME_DESIGN    1
#define MODERN_DESIGN  2
#define HACKER_DESIGN  3
#define PART1_DESIGN   4
#define PART2_DESIGN   5
#define PART3_DESIGN   6
#define PART4_DESIGN   7
byte currentDesign = DEFAULT_DESIGN;

#define UNKNOWN_PAGE   0
#define THEMES_PAGE    4
#define SETTINGS_PAGE  5
#define PORTS_PAGE     6
#define SUPPLY_PAGE    7
#define ELECTRO_PAGE   8
byte currentPage = UNKNOWN_PAGE;

#define GET           "GET /"
#define INDEX_STR     "index"
#define HTM_EXT       ".htm"

#define BASE_PIR      "pir"
#define BASE_CONTACT  "cont"
#define BASE_LITE     "lite"
#define BASE_MAC      "mac"
#define BASE_IP       "ip"
#define CHECKED_STR   "checked"
#define UNCHECKED_STR "unchecked"

// server answers

String makeAnswer(String content) {
  String s = "";
  s += F("HTTP/1.1 200 OK\n");
  s += F("Content-Type: ");
  s += content;
  s += F("\n");
  s += F("Connnection: close\n"); // "Connection: keep-alive\n"
  return s;
}

void sendHtmlAnswer(EthernetClient cl) {cl.println(makeAnswer(F("text/html")));}
void sendCssAnswer (EthernetClient cl) {cl.println(makeAnswer(F("text/css")));}
void sendJsAnswer  (EthernetClient cl) {cl.println(makeAnswer(F("application/javascript")));}
void sendPngAnswer (EthernetClient cl) {cl.println(makeAnswer(F("image/png")));}
void sendJpgAnswer (EthernetClient cl) {cl.println(makeAnswer(F("image/jpeg")));}
void sendGifAnswer (EthernetClient cl) {cl.println(makeAnswer(F("image/gif")));}
void sendXmlAnswer (EthernetClient cl) {cl.println(makeAnswer(F("text/xml")));}
void sendIcoAnswer (EthernetClient cl) {cl.println(makeAnswer(F("image/x-icon")));}

//void sendDownAnswer(EthernetClient cl) {cl.println(makeAnswer(F("application/octet-stream")));}
//void sendPdfAnswer (EthernetClient cl) {cl.println(makeAnswer(F("application/x-pdf")));}
//void sendZipAnswer (EthernetClient cl) {cl.println(makeAnswer(F("application/x-zip")));}
//void sendGzAnswer  (EthernetClient cl) {cl.println(makeAnswer(F("application/x-gzip")));}
//void sendElseAnswer(EthernetClient cl) {cl.println(makeAnswer(F("text/plain")));}

void sendErrorAnswer(char mess[], EthernetClient cl) {
  cl.print(mess);
  cl.println(F(" ERROR"));
  cl.println(F("Connnection: close"));
  cl.println();
}

String tagXmlVersion() {return F("<?xml version = \"1.0\" ?>");}
String openInputs()    {return F("\n<inputs>\n");}
String closeInputs()   {return F("</inputs>");}

// Tags

String makeTag(String tagBase, String tagCount, String value) {
  String s = "";
  s += "<"; s += tagBase; s += tagCount; s += ">";
  s += value;
  s += "</"; s += tagBase; s += tagCount; s += ">\n";
  return s;
}

boolean openWebFile() {
  char *fileName;
  fileName = strtok(HTTP_req, GET);
  webFile = SD.open(fileName);
  if (webFile) {return true;}
          else {return false;}
}

boolean openIndexFile(String s) {
  webFile = SD.open(INDEX_STR + s + HTM_EXT);
  if (webFile) {return true;}
          else {return false;}
}

String partSuffix(byte design) {
  switch (design) {
    case PART1_DESIGN:  return F("-p1"); break;
    case PART2_DESIGN:  return F("-p2"); break;
    case PART3_DESIGN:  return F("-p3"); break;
    case PART4_DESIGN:  return F("-p4"); break;
               default: return "";
  }
}

void parseRequest(EthernetClient cl) {
  allowMarkers = false;
  
  // index request
  if (StrContains(HTTP_req, "GET / ") || StrContains(HTTP_req, "GET /index.htm")) {
    if (openIndexFile(partSuffix(currentDesign))) {sendHtmlAnswer(cl);} else {webFile = SD.open(F("404.htm"));}
    allowMarkers = true;
  }
  else if (StrContains(HTTP_req, GET)) {
    // files requests
    if      (StrContains(HTTP_req, HTM_EXT)) {if (openWebFile()) {sendHtmlAnswer(cl);} else {webFile = SD.open(F("404.htm"));} allowMarkers = true;}
    else if (StrContains(HTTP_req, ".css"))  {if (openWebFile()) {sendCssAnswer(cl);}  else {sendErrorAnswer("", cl);}}
    else if (StrContains(HTTP_req, ".js"))   {if (openWebFile()) {sendJsAnswer(cl);}   else {sendErrorAnswer("", cl);}}
    else if (StrContains(HTTP_req, ".pde"))  {if (openWebFile()) {sendJsAnswer(cl);}   else {sendErrorAnswer("", cl);}}
    else if (StrContains(HTTP_req, ".png"))  {if (openWebFile()) {sendPngAnswer(cl);}  else {sendErrorAnswer("", cl);}}
    else if (StrContains(HTTP_req, ".jpg"))  {if (openWebFile()) {sendJpgAnswer(cl);}  else {sendErrorAnswer("", cl);}}
    else if (StrContains(HTTP_req, ".gif"))  {if (openWebFile()) {sendGifAnswer(cl);}  else {sendErrorAnswer("", cl);}}
    else if (StrContains(HTTP_req, ".ico"))  {if (openWebFile()) {sendIcoAnswer(cl);}  else {sendErrorAnswer("", cl);}}
    // Ajax requests
    else if (StrContains(HTTP_req, "request_dash"))    {sendXmlAnswer(cl); checkPage();      responseDash(cl);}
    else if (StrContains(HTTP_req, "request_generic")) {sendXmlAnswer(cl); setLeds();        responseGeneric(cl);}
    else if (StrContains(HTTP_req, "request_settings")){sendXmlAnswer(cl); setSettings();    responseSettings(cl);}
    else if (StrContains(HTTP_req, "request_themes"))  {sendXmlAnswer(cl); setTheme();       responseThemes(cl);}
    else if (StrContains(HTTP_req, "request_control")) {sendXmlAnswer(cl); setControl();     responseControl(cl);}
    else if (StrContains(HTTP_req, "request_noo"))     {sendXmlAnswer(cl); setNoo();         responseNoo(cl);}
    else if (StrContains(HTTP_req, "request_mr1132"))  {sendXmlAnswer(cl); setMr1132();      responseMr1132(cl);}
    else if (StrContains(HTTP_req, "req_el_control"))  {sendXmlAnswer(cl); setElectroCtrl(); responseElectroCtrl(cl);}
    else if (StrContains(HTTP_req, "req_el_freq"))     {sendXmlAnswer(cl); checkOscill();    responseElectroFreq(cl);}
    else if (StrContains(HTTP_req, "request_sdcard"))  {sendXmlAnswer(cl);                   responseSd(cl);}
    else if (StrContains(HTTP_req, "request_network")) {sendXmlAnswer(cl);                   responseNetwork(cl);}
    else if (StrContains(HTTP_req, "request_nrf24"))   {sendXmlAnswer(cl);                   responseNrf24(cl);}
  } // else if (StrContains(HTTP_req, GET))
} // parseRequest ( )

#ifdef CONTACTS_FEATURE
  String makeContacts() {
    String s = "";
    s += makeTag(BASE_CONTACT, "1", String(cont1objectState));
    s += makeTag(BASE_CONTACT, "2", String(cont2objectState));
    return s;
  }
#endif

#ifdef PIRS_FEATURE
  String makePirs() {
    String s = "";
    s += makeTag(BASE_PIR, "1", String(pir1));
    s += makeTag(BASE_PIR, "2", String(pir2));
    s += makeTag(BASE_PIR, "3", String(pir3));
    s += makeTag(BASE_PIR, "4", String(pir4));
    s += makeTag(BASE_PIR, "5", String(pir5));
    s += makeTag(BASE_PIR, "6", String(pir6));
    return s;
  }
#endif

#ifdef ELECTRO_FEATURE
  String makeElectro() {
    String s = "";
    byte digits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (byte i = 1; i < MAX_UI_SENSORS; i++) {
      s += makeTag("p", String(digits[i]), String(ajaxUPrms[i]));
    }
    s += makeTag("volt", "", String(ajaxUPrms[0]));
    return s;
  }
#endif

String makeModules() {
  String s = "";
  s += makeTag(F("mRtc"),      "", String(modulRtc));
  s += makeTag(F("mEthernet"), "", String(modulEthernet));
  s += makeTag(F("mSd"),       "", String(modulSdCard));
  s += makeTag(F("mServer"),   "", String(modulServer));
  s += makeTag(F("mMajor"),    "", String(modulMajor));
  s += makeTag(F("mLaurent"),  "", String(modulLaurent));
  s += makeTag(F("mUpload"),   "", String(modulUpload));
  s += makeTag(F("mPirs"),     "", String(modulPirs));
  s += makeTag(F("mContacts"), "", String(modulContacts));
  s += makeTag(F("mTemp"),     "", String(modulTemp));
  s += makeTag(F("mElectro"),  "", String(modulElectro));
  s += makeTag(F("mLeds"),     "", String(modulLeds));
  s += makeTag(F("mKeys"),     "", String(modulKeys));
  s += makeTag(F("mPing"),     "", String(modulPing));
  s += makeTag(F("mNoo"),      "", String(modulNoo));
  s += makeTag(F("mNrf24"),    "", String(modulNrf24));
  return s;
}

String makeDigitalPorts() {
  String s = "";
  byte pins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 22, 23, 24, 25, 26, 27, 30, 31};
  for (byte i = 0; i < sizeof(pins); i++) {
    s += makeTag("D", String(pins[i]), String(digitalRead(pins[i])));
  }
  return s;
}

String makeAnalogPorts() {
  String s = "";
  for (byte i = 0; i < MAX_ANALOG_PORTS; i++) {
    s += makeTag("A", String(i), String(analogRead(i)));
  }
  return s;
}

String makeDigits(int digits) {
  String s = "";
  if (digits < 10) {s += '0';}
  s += String(digits);
  return s;
}

String makeTime() {
  String s = "";
  s += "<time>";
    s += makeDigits(hour());
    if (second() % 2 == 0) {s += " ";}
                      else {s += ":";}
    s += makeDigits(minute());
  s += "</time>\n";
  return s;
}

String makeCpuLoad() {
  String s = "";
  s += makeTag("cycDelay", "", String(cyclosDelay));
  s += makeTag("cycInSec", "", String(cyclosInSec));
  return s;
}

String makeHttpReq() {
  String s = "";
  for (int i = 0; i < reqIndex; i++) {
    if (HTTP_req[i] == '&') {s += ' ';}
                       else {s += HTTP_req[i];}
  }
  return makeTag("httpReq", "", s);
}

#ifdef PING_FEATURE
  String makeDevicesOnline() {
    String s = "";
    for (byte i = 0; i < TOTAL_NET_DEVICES; i++) {
      s += makeTag(netDevicesNames[i], "", String(online[i]));
    }
    return s;
  }
#endif

// Generic

void setLeds() {
  // LED 1 (pin 6)
  if (StrContains(HTTP_req, "LED1=1")) {
    LED_state[0] = 1;  // save LED state
    digitalWrite(6, HIGH);
  } else if (StrContains(HTTP_req, "LED1=0")) {
      LED_state[0] = 0;  // save LED state
      digitalWrite(6, LOW);
    }

    // LED 2 (pin 7)
  if (StrContains(HTTP_req, "LED2=1")) {
    LED_state[1] = 1;
    digitalWrite(7, HIGH);
  } else if (StrContains(HTTP_req, "LED2=0")) {
      LED_state[1] = 0;
      digitalWrite(7, LOW);
    }

  // LED 3 (pin 5)
  if (StrContains(HTTP_req, "LED3=1")) {
    LED_state[2] = 1;
    digitalWrite(5, HIGH);
  } else if (StrContains(HTTP_req, "LED3=0")) {
      LED_state[2] = 0;
      digitalWrite(5, LOW);
    }

  // LED 4 (pin 3)
  if (StrContains(HTTP_req, "LED4=1")) {
    LED_state[3] = 1;
    digitalWrite(3, HIGH);
  } else if (StrContains(HTTP_req, "LED4=0")) {
      LED_state[3] = 0;
      digitalWrite(3, LOW);
    }
} // SetLEDs

void responseGeneric(EthernetClient cl) {
  int sw_arr[] = {42, 43, 45};
    
  String s = tagXmlVersion();
  s += openInputs();
    // analog inputs
    for (byte i = 0; i <= 5; i++) { // A2 to A5
      s += "<analog>";
        s += String(analogRead(i));
      s += "</analog>\n";
    }

    // HTTP_req
    s += makeHttpReq();

    // switches
    for (byte i = 0; i < 3; i++) {
      s += "<switch>";
        if (digitalRead(sw_arr[i])) {s += "ON";}
                               else {s += "OFF";}
      s += "</switch>\n";
    }
    
    // checkbox LED states    
    s += "<LED>"; // LED1 (switch)
      if (LED_state[0]) {s += CHECKED_STR;}
                   else {s += UNCHECKED_STR;}
    s += "</LED>\n";

    s += "<LED>"; // LED2 (switch)
      if (LED_state[1]) {s += CHECKED_STR;}
                   else {s += UNCHECKED_STR;}
    s += "</LED>\n";

    // button LED states
    s += "<LED>"; // LED3 (button)
      if (LED_state[2]) {s += "on";}
                   else {s += "off";}
    s += "</LED>\n";

    s += "<LED>"; // LED4 (button)
      if (LED_state[3]) {s += "on";}
                   else {s += "off";}
    s += "</LED>\n";
  s += closeInputs();
  cl.println(s);
}

// Network

void responseNetwork(EthernetClient cl) {
  String s = tagXmlVersion();
  s += openInputs();
    s += makeTag(BASE_MAC, "1", String(SELF_MAC[0], HEX));
    s += makeTag(BASE_MAC, "2", String(SELF_MAC[1], HEX));
    s += makeTag(BASE_MAC, "3", String(SELF_MAC[2], HEX));
    s += makeTag(BASE_MAC, "4", String(SELF_MAC[3], HEX));
    s += makeTag(BASE_MAC, "5", String(SELF_MAC[4], HEX));
    s += makeTag(BASE_MAC, "6", String(SELF_MAC[5], HEX));
    s += makeTag(BASE_IP,  "1", String(SELF_IP[0]));
    s += makeTag(BASE_IP,  "2", String(SELF_IP[1]));
    s += makeTag(BASE_IP,  "3", String(SELF_IP[2]));
    s += makeTag(BASE_IP,  "4", String(SELF_IP[3]));
    
    #ifdef PING_FEATURE
      makeDevicesOnline();
    #endif    
  s += closeInputs();
  cl.println(s);
}

// Electro

void setElectroCtrl() {
  // Modul & button Electro ON/OFF
  if (modulElectro != MODUL_NOT_COMPILLED) {
    if (StrContains(HTTP_req, "btn1=1")) {
      buttonElectro = 1;
      modulElectro = MODUL_ENABLE;
    } else if (StrContains(HTTP_req, "btn1=0")) {
      buttonElectro = 0;
      modulElectro = MODUL_DISABLE;
    }
  }
}

void responseElectroCtrl(EthernetClient cl) {
  String s = tagXmlVersion();
  s += openInputs();
    s += makeTag("modulElectro", "", String(modulElectro));
    
    s += "<LED>";
      if (buttonElectro) {s += "on";}
                    else {s += "off";}
    s += "</LED>";
    
  s += closeInputs();
  cl.println(s);
}

// Settings

void setSettings() {
  //if      (StrContains(HTTP_req, "box1=1")) {modeNetWork = MODE_MAJOR;} 
  //else if (StrContains(HTTP_req, "box1=0")) {modeNetWork = MODE_ONE;}

  if      (StrContains(HTTP_req, "box2=1")) {authMode = AUTH_ON;} 
  else if (StrContains(HTTP_req, "box2=0")) {authMode = AUTH_OFF;}
}

void responseSettings(EthernetClient cl) {
  String s1 = "";
  String s2 = "";

  //if (modeNetWork == MODE_MAJOR) {s1 = CHECKED_STR;} else {s1 = UNCHECKED_STR;}
  if (authMode    == AUTH_ON)    {s2 = CHECKED_STR;} else {s2 = UNCHECKED_STR;}
  
  String s = tagXmlVersion();
  s += openInputs();
    //s += makeTag("box1", "", s1);
    s += makeTag("box2", "", s2);
  s += closeInputs();
  cl.println(s);
}

// Themes

void setTheme() {
  if      (StrContains(HTTP_req, "theme=1")) {currentDesign = PART1_DESIGN;}
  else if (StrContains(HTTP_req, "theme=2")) {currentDesign = PART2_DESIGN;}
  else if (StrContains(HTTP_req, "theme=3")) {currentDesign = PART3_DESIGN;}
  else if (StrContains(HTTP_req, "theme=4")) {currentDesign = PART4_DESIGN;}  
  else if (StrContains(HTTP_req, "theme=5")) {currentDesign = DEFAULT_DESIGN;}
  else if (StrContains(HTTP_req, "theme=6")) {currentDesign = HOME_DESIGN;} 
  else if (StrContains(HTTP_req, "theme=7")) {currentDesign = MODERN_DESIGN;}
  else if (StrContains(HTTP_req, "theme=8")) {currentDesign = HACKER_DESIGN;}
}

String getThemeCode() {
  switch (currentDesign) {
    case PART1_DESIGN:   return "1"; break;
    case PART2_DESIGN:   return "2"; break;
    case PART3_DESIGN:   return "3"; break;
    case PART4_DESIGN:   return "4"; break;
    case DEFAULT_DESIGN: return "5"; break;
    case HOME_DESIGN:    return "6"; break;
    case MODERN_DESIGN:  return "7"; break;
    case HACKER_DESIGN:  return "8"; break;
                default: return "";
  }
}

void responseThemes(EthernetClient cl) {
  String s = tagXmlVersion();
  s += openInputs();
    s += makeTag("theme", "", getThemeCode());
  s += closeInputs();
  cl.println(s);
}

// Control

byte light1 = 0;
byte light2 = 0;
byte light3 = 0;
byte light4 = 0;
byte light5 = 0;

void setControl() {
  if (StrContains(HTTP_req, "light1=1")) {
    light1 = 1;
    red(led1, 127);
  } 
  else if (StrContains(HTTP_req, "light1=0")) {
    light1 = 0;
    black(led1);
  }

  if (StrContains(HTTP_req, "light2=1")) {
    light2 = 1;
    blue(led1, 127);
  } 
  else if (StrContains(HTTP_req, "light2=0")) {
    light2 = 0;
    black(led1);
  }

  if (StrContains(HTTP_req, "light3=1")) {
    light3 = 1;
    green(led1, 127);
  } 
  else if (StrContains(HTTP_req, "light3=0")) {
    light3 = 0;
    black(led1);
  }

  if (StrContains(HTTP_req, "light4=1")) {
    light4 = 1;
    magenta(led1, 127);
  } 
  else if (StrContains(HTTP_req, "light4=0")) {
    light4 = 0;
    black(led1);
  }

  if (StrContains(HTTP_req, "light5=1")) {
    light5 = 1;
    yellow(led1, 127);
  } 
  else if (StrContains(HTTP_req, "light5=0")) {
    light5 = 0;
    black(led1);
  }     
} // setControl()

void responseControl(EthernetClient cl) {
  String s = tagXmlVersion();
  s += openInputs();
    s += makeTag(BASE_LITE, "1", String(light1));
    s += makeTag(BASE_LITE, "2", String(light2));
    s += makeTag(BASE_LITE, "3", String(light3));
    s += makeTag(BASE_LITE, "4", String(light4));
    s += makeTag(BASE_LITE, "5", String(light5));
  s += closeInputs();
  cl.print(s);
}

// nooLite

#define STATE_OFF     0
#define STATE_ON      1
#define STATE_UNKNOWN 2

#define UNBIND        0
#define BIND          1
#define BIND_UNKNOWN  2

byte noo1state = STATE_OFF;
byte noo2state = STATE_OFF;
byte noo3state = STATE_OFF;

byte noo2multiCommand;
byte noo3multiCommand;

char *channelNum;

void setNoo() {
  // noo1
  if (StrContains(HTTP_req, "noo1state=1")) {
    noo1state = STATE_ON;
    nooOn(NOO_CHANNEL_1);
  } 
  else if (StrContains(HTTP_req, "noo1state=0")) {
    noo1state = STATE_OFF;
    nooOff(NOO_CHANNEL_1);
  }

  // noo2
  if (StrContains(HTTP_req, "noo2state=1")) {
    noo2state = STATE_ON;
    nooOn(NOO_CHANNEL_2);
  } 
  else if (StrContains(HTTP_req, "noo2state=0")) {
    noo2state = STATE_OFF;
    nooOff(NOO_CHANNEL_2);
  }  

  // noo3
  if (StrContains(HTTP_req, "noo3state=1")) {
    noo3state = STATE_ON;
    //nooOn(NOO_CHANNEL_3);
    nooSetLedLastState(NOO_CHANNEL_3);
  } 
  else if (StrContains(HTTP_req, "noo3state=0")) {
    noo3state = STATE_OFF;
    nooOff(NOO_CHANNEL_3);
  } 

  // bind/unbind
  byte pos;
  String commandStr;
  byte command;
  byte bind;

  if (StrContains(HTTP_req, "nooChannel=")) {
    pos = request.indexOf("nooChannel=") + 11;
    commandStr = request.substring(pos, pos + 2);
    command = commandStr.toInt();
  }

  if (StrContains(HTTP_req, "nooBind=1")) {
    bind = 1;
  } 
  else if (StrContains(HTTP_req, "nooBind=0")) {
    bind = 0;
  }
  
  if (command >= 0 && command <= 31) {
    if (bind == 1) {
      nooBind(command);
    } else {
        nooUnbind(command);
      }
    Serial.print("command: ");
    Serial.println(command);
    Serial.print("action: ");
    Serial.println(bind);
  }

  byte commandCorrect;

  // noo2multi
  if (StrContains(HTTP_req, "noo2multi=")) {
    pos = request.indexOf("noo2multi=") + 10;
    commandStr = request.substring(pos, pos + 2);
    noo2multiCommand = commandStr.toInt();

    if (noo2multiCommand >= 10 && noo2multiCommand <= 90) {
      commandCorrect = byte(noo2multiCommand * 1.2 + 34);
      nooLed(NOO_CHANNEL_1, 0, 0, 0);
      nooValue(NOO_CHANNEL_2, commandCorrect);
      Serial.print("(Multi) command: ");
      Serial.println(noo2multiCommand);
      Serial.print("(Multi) command correct: ");
      Serial.println(commandCorrect);      
    }    
  }

  // noo3multi
  if (StrContains(HTTP_req, "noo3multi=")) {
    pos = request.indexOf("noo3multi=") + 10;
    commandStr = request.substring(pos, pos + 2);
    noo3multiCommand = commandStr.toInt();

    if (noo3multiCommand >= 10 && noo3multiCommand <= 90) {
      switch (noo3multiCommand) {
        case 10: nooRed    (NOO_CHANNEL_3, 127); break;
        case 20: nooGreen  (NOO_CHANNEL_3, 127); break;
        case 30: nooBlue   (NOO_CHANNEL_3, 127); break;
        case 40: nooYellow (NOO_CHANNEL_3, 127); break;
        case 50: nooMagenta(NOO_CHANNEL_3, 127); break;
        case 60: nooCyan   (NOO_CHANNEL_3, 127); break;
        case 70: nooGray   (NOO_CHANNEL_3, 127); break;
        case 80: nooGray   (NOO_CHANNEL_3, 127); break;
        case 90: nooGray   (NOO_CHANNEL_3, 127); break;
        default: ;
      }
    }    
  }
} // setNoo()

void responseNoo(EthernetClient cl) {
  String s = tagXmlVersion();
  s += openInputs();
    s += makeTag("noo1", "state", String(noo1state));
    s += makeTag("noo2", "state", String(noo2state));
    s += makeTag("noo3", "state", String(noo3state));
    s += makeTag("noo2multi", "", String(noo2multiCommand));
    s += makeTag("noo3multi", "", String(noo3multiCommand));
  s += closeInputs();
  cl.println(s);
}

// nooLite MR1132

void setMr1132() {
  // bind/unbind
  byte pos;
  String commandStr;
  byte command;
  byte bind;

  if (StrContains(HTTP_req, "nooChannel=")) {
    pos = request.indexOf("nooChannel=") + 11;
    commandStr = request.substring(pos, pos + 2);
    command = commandStr.toInt();
  }

  if (StrContains(HTTP_req, "nooBind=1")) {
    bind = 1;
  } 
  else if (StrContains(HTTP_req, "nooBind=0")) {
    bind = 0;
  }

  #ifdef MR1132_FEATURE
    if (command >= 0 && command <= 31) {
      if (bind == 1) {
        mrBind(command);
      } else {
          mrUnbind(command);
        }
    }
  #endif
} // setMr1132()

void responseMr1132(EthernetClient cl) {
  String s = tagXmlVersion();
  s += openInputs();
  #ifdef MR1132_FEATURE
    s += makeTag("ns1", "Batt",     String(ns1BatteryState));
    s += makeTag("ns1", "Temp",     String(ns1Temp));
    s += makeTag("ns2", "Batt",     String(ns2BatteryState));
    s += makeTag("ns2", "Humidity", String(ns2Humidity));
    s += makeTag("ns2", "Temp",     String(ns2Temp));
    s += makeTag("ns1", "State",    String(np1State));
  #endif
  s += closeInputs();
  cl.println(s);
}

// nRF24

void responseNrf24(EthernetClient cl) {
  String s = tagXmlVersion();
  s += openInputs();
    s += makeTag("c1", "Name",            nrf1Name);
    s += makeTag("c1", "Id",              nrf1Id);
    s += makeTag("c1", "Addr",     String(nrf1Addr));
    s += makeTag("c1", "Batt",     String(nrf1Battery));
    s += makeTag("c1", "Health",   String(nrf1Health));
    s += makeTag("c1", "FreeMem",  String(nrf1FreeMem));
    s += makeTag("c1", "SelfTemp", String(nrf1SelfTemp));
    s += makeTag("c1", "Temp1",    String(nrf1Temp1));
  s += closeInputs();
  cl.println(s);
}

// Check page

void checkPage() {
  currentPage = UNKNOWN_PAGE;
  if      (StrContains(HTTP_req, "settings-page")) {currentPage = SETTINGS_PAGE;}
  else if (StrContains(HTTP_req, "themes-page"))   {currentPage = THEMES_PAGE;}
  else if (StrContains(HTTP_req, "ports-page"))    {currentPage = PORTS_PAGE;}
  else if (StrContains(HTTP_req, "supply-page"))   {currentPage = SUPPLY_PAGE;}
  else if (StrContains(HTTP_req, "electro-page"))  {currentPage = ELECTRO_PAGE;}  
}

// Oscill

void checkOscill() {
  if (StrContains(HTTP_req, "oscill")) {
    oscill = true;
  } else {
      oscill = false;
    }
}

void responseElectroFreq(EthernetClient cl) {
  String s = tagXmlVersion();
  s += openInputs();
    #ifdef ELECTRO_FEATURE
      for (int i = 0; i < MAX_FORM_POINTS; i++) {s += makeTag("u", "", String(pointsU[i]));}
      for (int i = 0; i < MAX_FORM_POINTS; i++) {s += makeTag("i", "", String(pointsI[i]));}    
                                               
    #endif 
   s += closeInputs();
  cl.println(s);
}

// SD card

void responseSd(EthernetClient cl) {
  String s = tagXmlVersion();
  s += openInputs();
    #ifdef SD_INFO_FEATURE
      s += makeTag("sdType",   "", String(sdType()));
      s += makeTag("sdSystem", "", String(sdFileSystem()));
      s += makeTag("sdSize",   "", String(sdVolumeSize()));
    #endif
    #ifdef SD_FILES_FEATURE
      //s += makeTag("sdRootDir","", s); // s.substring(0, 300)
    #endif    
  s += closeInputs();
  cl.println(s);
}

// Dash

void responseDash(EthernetClient cl) {
  String s = tagXmlVersion();
  s += openInputs();
    s += makeModules();
    s += makeTag("uptime",   "", uptimeString());
    s += makeTag("freeRAM",  "", String(freeMem()));
    s += makeCpuLoad();
    s += makeTag("lifer",    "", String(lifer));
    s += makeTime();
    s += makeTag("day",      "", String(day()));
    s += makeTag("month",    "", String(month()));
    s += makeTag("weekday",  "", String(weekday()));

    if (currentPage == PORTS_PAGE) {
      s += makeDigitalPorts();
      s += makeAnalogPorts();
    }
    #ifdef PIRS_FEATURE
      s += makePirs();
    #endif
    #ifdef CONTACTS_FEATURE
      s += makeContacts();
    #endif  
    #ifdef PING_FEATURE
      makeDevicesOnline();
    #endif
    #ifdef LAURENT_FEATURE
      s += makeTag("laurent", "", lrequest);
    #endif
    #ifdef TEMP_FEATURE
      for (byte i = 0; i < MAX_TEMP_SENSORS; i++) {
        s += makeTag(objSens[i], "",       String(current_temp[i]));
        s += makeTag(objSens[i], "Health", String(tempSensorHealth[i]));
      }
      s += makeTag("mdTempHealth", "", String(tempModulHealth));     
    #endif
    #ifdef ELECTRO_FEATURE
      s += makeTag("voltage", "", String(ajaxUPrms[0]));
      s += makeTag("power",   "", String(ajaxUPrms[1]));
      if (currentPage == ELECTRO_PAGE) {
        s += makeElectro();
        s += makeTag("modulElectro", "", String(modulElectro));
      }
      if (currentPage == SUPPLY_PAGE) {
        s += makeTag("period", "", String(periodInMicros));
      }
    #endif

    s += makeTag("timeProv", "", String(timeProvider));
    s += makeTag("auth",     "", String(authMode));
    if (freeSockets == FREE_SOCKETS_ZERO) {
      s += makeTag("force", "", "1");
    } else {
        s += makeTag("force", "", "0");
      }
      
  s += closeInputs();
  cl.println(s);
}

#endif // SERVER_FEATURE
