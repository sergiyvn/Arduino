/*
  Modul RTC
  part of Arduino Mega Server project
*/

void rtcInit() {
  Serialprint("Modul RTC...\n");
  if (timeProvider == TIME_NETWORK) {
    Serialprint(" Waiting for NTP sync...\n");
    setSyncProvider(getNtpTime);
  }
  else if (timeProvider == TIME_RTC) {
    Serialprint(" Waiting for RTC sync...\n");
    setSyncProvider(RTC.get); // get time from RTC
  } 
  else {
    Serialprint(" RTC service stopped\n");
  }
  modulRtc = MODUL_ENABLE;
  initDone(true);
}

void rtcSync() {
  setSyncProvider(getNtpTime);
  Serialprint("...getNtpTime... \n");
  if (timeStatus() != timeNotSet) {
    Serialprint("...set!... \n");
    time_t t = getNtpTime();
    RTC.set(t);
    setSyncProvider(RTC.get);
  }
}

void printDigits(int digits) {
  if(digits < 10) {
    Serial.print('0');
  }
  Serial.print(digits);
}

void serialRTC() {
  Serial.print(year()); 
  Serial.print("-");
  printDigits(month());
  Serial.print("-");
  printDigits(day());
  Serial.print(" ");
  printDigits(hour());
  Serial.print(":");
  printDigits(minute());
  Serial.print(":");
  printDigits(second());
  //Serial.print(" ");
  //Serial.print(cyclos);
}

void timeStamp() {
  serialRTC();
  Serial.print(" ");
}

void printRTC(){
  serialRTC();
  Serial.println();
}

// Duration

void showDuration(time_t duration) {
  // prints the duration in days, hours, minutes and seconds
  timeStamp();
  Serialprint("(duration ");
  if(duration >= SECS_PER_DAY){
     Serial.print(duration / SECS_PER_DAY);
     Serialprint(" day "); 
     duration = duration % SECS_PER_DAY;     
  }
  if(duration >= SECS_PER_HOUR){
     Serial.print(duration / SECS_PER_HOUR);
     Serialprint(" hour "); 
     duration = duration % SECS_PER_HOUR;     
  }
  if(duration >= SECS_PER_MIN){
     Serial.print(duration / SECS_PER_MIN);
     Serialprint(" min "); 
     duration = duration % SECS_PER_MIN;     
  }
  Serial.print(duration);
  Serialprint(" sec)\n");   
}

void checkEvent(time_t* prevEvent) {
  time_t duration = 0;
  time_t timeNow = now();
  
  if (*prevEvent > 0) {
    duration = timeNow - *prevEvent;
  }     
  if (duration > 0) {
    showDuration(duration);
  }  
  *prevEvent = timeNow;
}

// Uptime

/* ----------------------------------------------------------
    Auto reset ~50 days
    TODO: save to EEPROM x50 days periods
------------------------------------------------------------- */

String uptimeString() {
  char uptime[14];
  char zero[] = "0";
  char nop[]  = "";
  char* zh;
  char* zm;
  char* zs;

  unsigned long totalSeconds = millis() / 1000;
  unsigned long days = totalSeconds / 86400;
  unsigned long tsHours = totalSeconds - days * 86400;
  unsigned long hours = tsHours / 3600;
  unsigned long tsMinutes = tsHours - hours * 3600;
  unsigned long minutes = tsMinutes / 60;
  unsigned long seconds = tsMinutes - minutes * 60;

  if (hours   < 10) {zh = zero;} else {zh = nop;}
  if (minutes < 10) {zm = zero;} else {zm = nop;}
  if (seconds < 10) {zs = zero;} else {zs = nop;}

  sprintf(uptime, "%d %s%d:%s%d:%s%d", (int)days, zh, (int)hours, zm, (int)minutes, zs, (int)seconds);
  return String(uptime);
} // uptimeString()

// Lifer

byte lifer;

void setLifer() {
  lifer++;
  if (lifer > 6) {
    lifer = 0;
  }
}

#ifdef NTP_FEATURE
  void setNtpInterval() {
    if (timeStatus() == timeNotSet) {
      setSyncInterval(18);
    } else {
        //setSyncInterval(3600); // not work > 19 sec. (cycle20s)
      }
  }
#endif

void rtcWorks() {
  #ifdef NTP_FEATURE
    if (cycle20s) {setNtpInterval();}
  #endif
  if (cycle1s) {setLifer();}
}

