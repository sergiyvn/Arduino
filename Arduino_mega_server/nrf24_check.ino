/*
  Modul nRF24 Check
  part of Arduino Mega Server project
*/

#ifdef NRF24_FEATURE

void resetSender(struct Sender *p) {
  p->active   = false;
  p->required = true;
  p->success  = true;
  p->tryouts  = 0;
  p->type     = NOT_DEF;
  setNextNumber(p);
}

// Echo

boolean matchEcho(struct Sender *p) {
  if (rxNumber != p->number) {
    return false;
  }
  return true;
}

void checkEcho(struct Sender *p) {
  if (matchEcho(p)) {
    resetSender(p);
    successPackets++;
    #ifdef SERIAL_PRINT
      Serial.print(F("ECHO: "));
      Serial.print(p->command);
      Serial.println(F(" DONE"));
    #endif
  }
}

void parseEcho() {
  switch (rxCommand) {
    case GET_NAME:      checkEcho(&sendGetName);     break;
    case GET_ID:        checkEcho(&sendGetId);       break;
    case GET_ADDR:      checkEcho(&sendGetAddress);  break;
    case GET_FREE_MEM:  checkEcho(&sendGetFreeMem);  break;
    case GET_BATTERY:   checkEcho(&sendGetBattery);  break;
    case GET_SELF_TEMP: checkEcho(&sendGetSelfTemp); break;
    case GET_HEALTH:    checkEcho(&sendGetHealth);   break;
    case GET_TEMP_1:    checkEcho(&sendGetTemp1);    break;    
  }
}

// Answers

void checkAnswer(struct Sender *p) {
  resetSender(p);
  successPackets++;
  #ifdef SERIAL_PRINT
    Serial.print(F("ANSWER: "));
    Serial.print(p->command);
    Serial.println(F(" DONE"));
  #endif  
}

void parseAnswers() {
  switch (rxCommand) {
    case DTA_NAME:
      checkAnswer(&sendGetName);
      nrf1Name = sendGetName.result;
      break;
    case DTA_ID:
      checkAnswer(&sendGetId);
      nrf1Id = sendGetId.result;
      break;
    case DTA_ADDR:
      checkAnswer(&sendGetAddress);
      nrf1Addr = sendGetAddress.result.toInt();
      break;
    case DTA_FREE_MEM:
      checkAnswer(&sendGetFreeMem);
      nrf1FreeMem = sendGetFreeMem.result.toInt();
    case DTA_BATTERY:
      checkAnswer(&sendGetBattery);
      nrf1Battery = sendGetBattery.result.toInt();
      break;
    case DTA_SELF_TEMP:
      checkAnswer(&sendGetSelfTemp);
      nrf1SelfTemp = sendGetSelfTemp.result.toFloat();
      break;
    case DTA_HEALTH:
      checkAnswer(&sendGetHealth);
      nrf1Health = sendGetHealth.result.toInt();
      break;
    case DTA_TEMP_1:
      checkAnswer(&sendGetTemp1);
      nrf1Temp1 = sendGetTemp1.result.toFloat();
      break;
  }
}

#endif // NRF24_FEATURE
