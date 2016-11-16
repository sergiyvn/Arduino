/*
  Modul Pages Maker
  part of Arduino Mega Server project
*/

#define NORMAL        0
#define ALL_SCRIPTS   1
#define ALT_CSS_STYLE 2

#define OPEN_BLOCK_STYLE  "\n<style>\n"
#define CLOSE_BLOCK_STYLE "</style>\n"
#define CLOSE_STYLE       "  }\n"

File insertFile;
#define MAX_BUFFER_INSERT 256
uint16_t sizeInsert;
uint8_t buffInsert[MAX_BUFFER_INSERT];


String ipString(byte ip[]) {
  String s = "";
  for (byte i = 0; i < 4; i++) {
    s += ip[i];
    if (i == 3) {return s;}
    s += '.';
  }
}

String openStyle(String style) {
  String s = F("  .");
  s += style;
  s += F(" {\n");
  return s;
}

// <style>
//   .style {
//     background: url(//ip/fileName) no-repeat;
//   }
// </style>"

String backgroundIpAttr(String fileName) {
  String s = F("    ");
  s += F("background: url(//");
  s += ipString(SELF_IP);
  s += F("/");
  s += fileName;
  s += F(") no-repeat;\n");
  return s;
}

void addBackgroundStyle(String style, String file, EthernetClient cl) {
  String s = OPEN_BLOCK_STYLE;
  s += openStyle(style);
  s += backgroundIpAttr(file);
  s += CLOSE_STYLE;
  s += CLOSE_BLOCK_STYLE;
  cl.println(s);
}

String themeSuffix(byte design) {
  switch (design) {
    case HOME_DESIGN:   return F("_hm"); break;
    case MODERN_DESIGN: return F("_md"); break;
    case HACKER_DESIGN: return F("_hk"); break;
    case PART1_DESIGN:  return F("_p1"); break;
    case PART2_DESIGN:  return F("_p2"); break;
    case PART3_DESIGN:  return F("_p3"); break;
    case PART4_DESIGN:  return F("_p4"); break;
               default: return "";
  }
}

void insert(File file, EthernetClient cl) {
  while (file.available()) {
    sizeInsert = file.read(buffInsert, MAX_BUFFER_INSERT);
    cl.write(buffInsert, sizeInsert);
  }
}

void insertThis(String fileNname, EthernetClient cl) {
  insertFile = SD.open(fileNname);
  if (insertFile) {
    insert(insertFile, cl);
    insertFile.close();
  }
}

void insertBlock(uint8_t operation, EthernetClient cl) {
  byte insertMode = NORMAL;
  String s = "";
  switch (operation) {
    case LINKS:
      s = themeSuffix(currentDesign);
      if (s == "_hm") {addBackgroundStyle(F("home"),   F("home.jpg"),   cl);}
      if (s == "_md") {addBackgroundStyle(F("modern"), F("modern.jpg"), cl);}
      insertFile = SD.open("_one" + s + HTM_EXT);
      break;
    case HEADER:     s = themeSuffix(currentDesign); if (s == "") {s = "er";} insertFile = SD.open("_head" + s + HTM_EXT); break;
    case FOOTER:     s = themeSuffix(currentDesign); if (s == "") {s = "er";} insertFile = SD.open("_foot" + s + HTM_EXT); break;
    case BANNERS:    if (random(1, 15) == 5) {insertFile = SD.open(F("_banner.htm"));} break;
    case DASH:       insertFile = SD.open("_dash" + themeSuffix(currentDesign) + HTM_EXT); break;
    case MENU:       insertFile = SD.open("_menu" + themeSuffix(currentDesign) + HTM_EXT); break;
    case ALT_MENU:   insertFile = SD.open("_alt"  + themeSuffix(currentDesign) + HTM_EXT); break;
    case ADDRESS:    cl.print(ipString(SELF_IP));           break;
    case SCRIPTS:    insertMode = ALL_SCRIPTS;              break;
    case ALT_CSS:    insertMode = ALT_CSS_STYLE;            break;
    case FLOTR2:     insertFile = SD.open(F("flotr2.js"));  break;
    case PROCESSING: insertFile = SD.open(F("process.js")); break;
    case THREE:      insertFile = SD.open(F("three.js"));   break;
    case JQUERY:     insertFile = SD.open(F("jquery.js"));  break;
    default: 
      {}
  } // switch
  /*
  if (insertFile) {
    while (insertFile.available()) {
      sizeInsert = insertFile.read(buffInsert, MAX_BUFFER_INSERT);
      cl.write(buffInsert, sizeInsert);
    }
    insertFile.close();
  }
  */
  switch (insertMode) {
    case NORMAL:
      if (insertFile) {
        insert(insertFile, cl);
        insertFile.close();
      }
      break;
    case ALL_SCRIPTS:
      insertThis(F("graphics.js"), cl);
      insertThis(F("scripts.js"),  cl);
      insertThis(F("dash.js"),     cl);    
      break;
    case ALT_CSS_STYLE:
      insertThis(F("style_a1.css"), cl); 
      break;
    default: 
      {}
  } //switch (insertMode)
  
} // insertBlock( )

