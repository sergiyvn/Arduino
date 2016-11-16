/*
  Modul SD card
  part of Arduino Mega Server project
*/

#define SD_CHIP_SELECT 4

File sdroot;
int countRoot  = 0;
int emptyFiles = 0;
long sizeRoot  = 0;

void sdCardInit() {
  Serialprint("Modul SD...\n");
  
  Serialprint(" SD init... ");
  if (SD.begin(SD_CHIP_SELECT)) {
    Serialprint("OK\n");
    
    if (SD.exists("index.htm")) {
      Serialprint(" index.htm... found\n");
    } else {
        Serialprint(" Can't find index.htm\n");
        return;      
      }
  } else {
      Serialprint("failed\n");
      return;
    }
 
  sdroot = SD.open("/");
  checkDir(sdroot);
  Serialprint(" Files: "); Serial.println(countRoot);
  Serialprint(" Size:  "); Serial.println(sizeRoot);
  if (emptyFiles) {
    Serialprint(" Empty: "); Serial.println(emptyFiles);
  }
  
  modulSdCard = MODUL_ENABLE;
  initDone(false);
} // sdCardInit()

void checkDir(File dir) {
  countRoot  = 0;
  sizeRoot   = 0;
  emptyFiles = 0;
  Serialprint(" Check");
  dir.rewindDirectory();
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {break;}
    countRoot++;
    if (countRoot %  10 == 0) {Serialprint(".");}
    if (countRoot % 400 == 0) {Serialprint("\n");}
    if (entry.size() == 0) {emptyFiles++;}
    sizeRoot += entry.size();
    entry.close();
  }
 Serialprint("\n");
}

#ifdef SD_INFO_FEATURE
  Sd2Card  card;
  SdVolume volume;
  SdFile   root;

  void sdInfoInit() {
    Serialprint("Modul SD Info... \n");
    if (card.init(SPI_FULL_SPEED, SD_CHIP_SELECT)) {
      printSdType();
      printFsType();
      printSdSize();
      //printSdRoot();
      initDone(false);
    } else {
        Serialprint("failed\n");
      }
  }

  // SD info
  
  byte sdType() {
    switch(card.type()) {
      case SD_CARD_TYPE_SD1:  return 1; break;
      case SD_CARD_TYPE_SD2:  return 2; break;
      case SD_CARD_TYPE_SDHC: return 3; break;
                     default: return 0;
    }
  }

  byte sdFileSystem() {
    if (volume.init(card)) {return volume.fatType();}
                      else {return 0;}
  }

  int sdVolumeSize() {
    int k = 1000; // 1000 or 1024
    if (volume.init(card)) {
      double volumesize;
      volumesize = volume.blocksPerCluster();
      volumesize *= volume.clusterCount();
      volumesize *= 512;      // size in bytes
      volumesize /= k;        // size in Kbytes
      return volumesize /= k; // size in Mbytes
    } else {
        return 0;
      }
  }

  // print SD info

  boolean sdIsInit() {
    // try to open the partition FAT16 or FAT32
    if (volume.init(card)) {
      return true;
    } else {
        Serialprint("Not find partition\n");
        return false;
      }
  }
  
  void printSdType() {
    Serialprint(" SD type:    ");
    switch(card.type()) {
      case SD_CARD_TYPE_SD1:  Serialprint("SD1"); break;
      case SD_CARD_TYPE_SD2:  Serialprint("SD2"); break;
      case SD_CARD_TYPE_SDHC: Serialprint("SDHC");break;
                     default: Serialprint("Unknown");
    }
    Serialprint("\n");
  }

  void printFsType() {
    // type of the first FAT-type volume
    if (sdIsInit()) {
      Serialprint(" Filesystem: FAT");
      Serial.println(volume.fatType(), DEC);
    }
  }
  
  void printSdSize() {
    int k = 1000; // 1000 or 1024
    // try to open the partition FAT16 or FAT32
    if (sdIsInit()) {
      double volumesize;
      volumesize = volume.blocksPerCluster();
      volumesize *= volume.clusterCount();
      volumesize *= 512; // SD card blocks are always 512 bytes

      Serialprint(" Cluster:    ");
      Serial.println(volume.blocksPerCluster() * 512);
      
      //Serialprint("Volume size (B):  ");
      //Serial.println(volumesize);
      
      volumesize /= k;
      //Serialprint("Volume size (KB): ");
      //Serial.println(volumesize);
      
      volumesize /= k;
      //Serialprint("Volume size (MB): ");
      //Serial.println(volumesize);

      int intSize = volumesize;
      
      Serialprint(" SD size:    ");
      Serial.print(intSize); Serialprint(" MB\n");
    } // if (sdIsInit())
  } // printSdSize()

  void printSdRoot() {
    // try to open the partition FAT16 or FAT32
    if (sdIsInit()) {
      Serialprint("Found files:\n");
      root.openRoot(volume);
      root.ls(LS_R | LS_DATE | LS_SIZE);
    }
  }
#endif // SD_INFO_FEATURE

#ifdef SD_FILES_FEATURE
  void sdFilesInit() {
    started("SD Files", false);
  }

  void printSdContent(File dir, int numTabs) {
     while(true) {
       File entry =  dir.openNextFile();
       if (! entry) {
         dir.rewindDirectory();
         break;
       } // no more files
       for (uint8_t i = 0; i < numTabs; i++) {
         Serial.print('\t');
       }
       Serial.print(entry.name());
       if (entry.isDirectory()) {
         Serialprint("/\n");
         printSdContent(entry, numTabs + 1);
       } else {
           Serial.print("\t");
           Serial.println(entry.size(), DEC);
         }
       entry.close();
     }
  }  // printSdContent( )

  void printDirectory(File dir) {
     while(true) {
       File entry = dir.openNextFile();
       if (!entry) {break;} // no more files
       if (!entry.isDirectory()) {
         Serial.println(entry.name());
       } 
       entry.close();
     }
  }

  String strDirectory(File dir) {
    String str = "";
    File entry;
    dir.rewindDirectory();
    while(true) {
      entry = dir.openNextFile();
      if (! entry) { // no more files
        dir.rewindDirectory();
        return str;
        break;
      }
      if (! entry.isDirectory()) {
        str += entry.name();
        str += " ";
      } 
      entry.close();
    }
  }
#endif // SD_FILES_FEATURE
