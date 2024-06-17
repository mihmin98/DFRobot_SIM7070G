#include <DFRobot_SIM7070G.h>

DFRobot_SIM7070G::DFRobot_SIM7070G(Stream* s) :DFRobot_SIMcore(s)
{
  _s = s;
}

bool DFRobot_SIM7070G::setBaudRate(long rate)
{
  int  count = 0;
  while (count < 3) {
    if (rate == 1200) {
      if (checkSendCmd("AT+IPR=1200\r\n", "OK")) {
        _baudrate = 1200;
        break;
      } else {
        count++;
        delay(200);
      }
    } else if (rate == 2400) {
      if (checkSendCmd("AT+IPR=2400\r\n", "OK")) {
        _baudrate = 2400;
        break;
      } else {
        count++;
        delay(200);
      }
    } else if (rate == 4800) {
      if (checkSendCmd("AT+IPR=4800\r\n", "OK")) {
        _baudrate = 4800;
        break;
      } else {
        count++;
        delay(200);
      }
    } else if (rate == 9600) {
      if (checkSendCmd("AT+IPR=9600\r\n", "OK")) {
        _baudrate = 9600;
        break;
      } else {
        count++;
        delay(200);
      }
    } else if (rate == 19200) {
      if (checkSendCmd("AT+IPR=19200\r\n", "OK")) {
        _baudrate = 19200;
        break;
      } else {
        count++;
        delay(200);
      }
    } else if (rate == 38400) {
      if (checkSendCmd("AT+IPR=38400\r\n", "OK")) {
        _baudrate = 38400;
        break;
      } else {
        count++;
        delay(200);
      }
    } else {
      Serial.println("No such rate");
      return false;
    }
  }
  if (count == 3)
    return false;
  return true;
}

bool DFRobot_SIM7070G::checkSIMStatus(void)
{
  int count = 0;
  while (count < 3) {
    if (checkSendCmd("AT\r\n", "OK")) {
      break;
    } else {
      count++;
      delay(300);
    }
  }
  if (count == 3) {
    return false;
  }
  count = 0;
  while (count < 3) {
    if (checkSendCmd("AT+CPIN?\r\n", "READY")) {
      break;
    } else {
      count++;
      delay(300);
    }
  }
  if (count == 3)
    return false;
  return true;
}

bool DFRobot_SIM7070G::setNetMode(eNet net)
{
  if (net == eNB) {
    _mode_t = 0;
    if (checkSendCmd("AT+CNMP=38\r\n", "OK")) {
      delay(300);
      if (checkSendCmd("AT+CMNB=2\r\n", "OK")) {
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else if (net == eGPRS) {
    _mode_t = 1;
    if (checkSendCmd("AT+CNMP=13\r\n", "OK")) {
      delay(300);
      if (checkSendCmd("AT+CMNB=1\r\n", "OK")) {
        return true;
      } else {
        return false;
      }
    }
  } else {
    Serial.println("No such mode!");
  }
  return false;
}

bool DFRobot_SIM7070G::attacthService(void)
{
  char gprsBuffer[32];
  cleanBuffer(gprsBuffer, 32);
  sendCmd("AT+CGATT=1\r\n");   // Attach or Detach from GPRS Service
  while (1) {
    readBuffer(gprsBuffer, 320);
    if (NULL != strstr(gprsBuffer, "OK")) {
      delay(100);
      break;
    }
    if (NULL != strstr(gprsBuffer, "ERROR")) {
      return false;
    }
  }

  cleanBuffer(gprsBuffer, 32);
  // if(_mode_t){
  //   sendCmd("AT+CSTT=\"cmnet\"\r\n");
  // }else{
  //   sendCmd("AT+CSTT=\"ctnb\"\r\n");  
  // }
  // if(_mode_t){
  //   sendCmd("AT+CNCFG=0,1,\"cmnet\"\r\n");
  // }else{
  //   sendCmd("AT+CNCFG=0,1,\"ctnb\"\r\n");  
  // }
  sendCmd("AT+CNACT=0,1\r\n");
  while (1) {
    readBuffer(gprsBuffer, 32);
    if (NULL != strstr(gprsBuffer, "OK")) {
      delay(200);
      break;
    } else if (NULL != strstr(gprsBuffer, "ERROR")) {
      return false;
    }
  }

  return true;
}

int DFRobot_SIM7070G::checkSignalQuality(void)
{
  char  signalBuffer[26];
  int i = 0, j = 0, k = 0;
  char* signalQuality;
  cleanBuffer(signalBuffer, 26);
  sendCmd("AT+CSQ\r\n");
  readBuffer(signalBuffer, 26);
  if (NULL != (signalQuality = strstr(signalBuffer, "+CSQ:"))) {
    i = *(signalQuality + 6) - 48;
    j = *(signalQuality + 7) - 48;
    k = (i * 10) + j;
  } else {
    return 0;
  }
  if (k == 99) {
    return 0;
  } else {
    return k;
  }
}

bool DFRobot_SIM7070G::openNetwork(eProtocol ptl, const char* host, uint16_t port)
{
  char num[8] = { "\0" };
  char resp[96];
  if (ptl == eTCP) {
    sendCmd("AT+CAOPEN=0,0,\"TCP\",\"");
    // sendCmd("AT+CIPSTART=\"TCP\",\"");
  } else if (ptl == eUDP) {
    sendCmd("AT+CAOPEN=0,0,\"UDP\",\"");
    // sendCmd("AT+CIPSTART=\"UDP\",\"");
  } else {
    Serial.println("No such mode!");
    return false;
  }
  sendCmd(host);
  sendCmd("\",");
  // itoa(port, num, 10);   // char *itoa(int i,char *s,int radix);   uno int = int16_t
  sprintf(num, "%u", port);
  sendCmd(num);
  sendCmd("\r\n");

  while (1) {
    while (checkReadable()) {
      cleanBuffer(resp, 96);
      readBuffer(resp, 96);
      if (NULL != strstr(resp, "+CAOPEN: 0,")) {
        if (NULL != strstr(resp, "+CAOPEN: 0,0")) {
          return true;
        } else {
          return false;
        }
      }
      if (NULL != strstr(resp, "ERROR")) {
        return false;
      }
    }
  }
}

bool DFRobot_SIM7070G::turnON(void)
{
  pinMode(12, OUTPUT);
  if (checkSendCmd("AT\r\n", "OK", 100)) {
    checkSendCmd("AT+CPOWD=1\r\n", "NORMAL POWER DOWN");
    delay(4000);
  }
  delay(100);
  digitalWrite(12, HIGH);
  delay(1000);
  digitalWrite(12, LOW);
  delay(7000);
  if (checkSendCmd("AT\r\n", "OK")) {
    return true;
  } else {
    delay(100);
    digitalWrite(12, HIGH);
    delay(1000);
    digitalWrite(12, LOW);
    delay(7000);
    return false;
  }
}

bool DFRobot_SIM7070G::initPos(void)
{
  if (checkSendCmd("AT+CGNSPWR=1\r\n", "OK")) {
    delay(50);
    return true;
  } else {
    return false;
  }
}

bool DFRobot_SIM7070G::send(char* data)
{
  char num[8] = { "\0" };
  char resp[20];
  int len = strlen(data);
  itoa(len, num, 10);
  sendCmd("AT+CASEND=0,");
  sendCmd(num);
  if (checkSendCmd("\r\n", ">")) {
    sendCmd(data);
    while (1) {
      while (checkReadable()) {
        readBuffer(resp, 20);
        if (NULL != strstr(resp, "OK")) {
          return true;
        }
        if (NULL != strstr(resp, "ERROR")) {
          return false;
        }
      }
    }
  } else {
    return false;
  }
}

bool DFRobot_SIM7070G::send(char* buf, size_t len)
{
  char num[8] = { "\0" };
  itoa(len, num, 10);
  sendCmd("AT+CASEND=0,");
  sendCmd(num);
  if (checkSendCmd("\r\n", ">")) {
    if (checkSendCmd(buf, "OK")) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool DFRobot_SIM7070G::mqttConnect(const char* iot_client, const char* iot_username, const char* iot_key)
{
  char num[8] = { "\0" };
  char resp[20];
  int len = 2 + 10 + 2 + strlen(iot_client) + 2 + strlen(iot_username) + 2 + strlen(iot_key);
  itoa(len, num, 10);
  sendCmd("AT+CASEND=0,");
  sendCmd(num);
  if (checkSendCmd("\r\n", ">")) {
    char MQTThead[10] = { 0x00,0x04,0x4d,0x51,0x54,0x54,0x04,(char)0xC2,0x0b,(char)0xb8 };
    char MQTTbuff[50] = { 0 };
    MQTTbuff[0] = 0x10;
    sendBuff(MQTTbuff, 1);
    int leng = 10;
    leng += strlen(iot_client) + 2;
    leng += strlen(iot_username) + 2;
    leng += strlen(iot_key) + 2;
    MQTTbuff[0] = leng;
    sendBuff(MQTTbuff, 1);
    sendBuff(MQTThead, 10);
    sendBuff(MQTThead, 1);
    MQTTbuff[0] = strlen(iot_client);
    sendBuff(MQTTbuff, 1);
    sendCmd(iot_client);
    sendBuff(MQTThead, 1);
    MQTTbuff[0] = strlen(iot_username);
    sendBuff(MQTTbuff, 1);
    sendCmd(iot_username);
    sendBuff(MQTThead, 1);
    MQTTbuff[0] = strlen(iot_key);
    sendBuff(MQTTbuff, 1);
    sendCmd(iot_key);
    while (1) {
      while (checkReadable()) {
        readBuffer(resp, 20);
        if (NULL != strstr(resp, "OK")) {
          return true;
        }
        if (NULL != strstr(resp, "ERROR")) {
          return false;
        }
      }
    }
  }
  return false;
}

bool DFRobot_SIM7070G::mqttPublish(const char* iot_topic, String iot_data)
{
  char num[8] = { "\0" };
  char resp[20];
  int len = 4 + strlen(iot_topic) + 2 + iot_data.length() - 1;
  itoa(len, num, 10);
  sendCmd("AT+CASEND=0,");
  sendCmd(num);
  if (checkSendCmd("\r\n", ">")) {
    DBG("aa");
    char MQTTdata[2] = { 0x00,0x04 };
    char MQTTbuff[50] = { 0 };
    MQTTbuff[0] = 0x32;
    sendBuff(MQTTbuff, 1);
    MQTTbuff[0] = strlen(iot_topic) + iot_data.length() - 1 + 4;
    sendBuff(MQTTbuff, 2);
    MQTTbuff[0] = strlen(iot_topic);
    sendBuff(MQTTbuff, 1);
    sendCmd(iot_topic);
    sendBuff(MQTTdata, 2);
    iot_data.toCharArray(MQTTbuff, iot_data.length());
    sendString(iot_data.c_str());

    while (1) {
      while (checkReadable()) {
        readBuffer(resp, 20);
        if (NULL != strstr(resp, "OK")) {
          delay(400);
          return true;
        }
        if (NULL != strstr(resp, "ERROR")) {
          return false;
        }
      }
    }
  } else {
    return false;
  }
}

bool DFRobot_SIM7070G::mqttSubscribe(const char* iot_topic)
{
  if (checkSendCmd("AT+CASEND=0,100\r\n", ">")) {
    char MQTTbuff[10] = { 0 };
    MQTTbuff[0] = 0x82;
    MQTTbuff[1] = strlen(iot_topic) + 5;
    MQTTbuff[3] = 0x0a;
    MQTTbuff[5] = strlen(iot_topic);
    sendBuff(MQTTbuff, 6);
    sendCmd(iot_topic);
    MQTTbuff[0] = 0x01;
    sendBuff(MQTTbuff, 1);
    if (!checkSendCmd("", "OK")) {
      return false;
    } else {
      return true;
    }
  }
  return false;
}

bool DFRobot_SIM7070G::mqttUnsubscribe(const char* iot_topic)
{
  if (checkSendCmd("AT+CASEND=0,100\r\n", ">")) {
    char MQTTbuff[10] = { 0 };
    MQTTbuff[0] = 0xa2;
    MQTTbuff[1] = strlen(iot_topic) + 4;
    MQTTbuff[3] = 0x0a;
    MQTTbuff[5] = strlen(iot_topic);
    sendBuff(MQTTbuff, 6);
    sendCmd(iot_topic);
    if (!checkSendCmd("", "OK")) {
      return false;
    } else {
      return true;
    }
  } else {
    return false;
  }
}

bool DFRobot_SIM7070G::mqttRecv(char* iot_topic, char* buf, int maxlen)
{
  char MQTTbuff[maxlen + 30];
  char* p;
  cleanBuffer(MQTTbuff, maxlen + 30);
  // int i = readBuffer(MQTTbuff, maxlen + 30);
  int i = recv(MQTTbuff, maxlen + 30);
  for (int j = 0;j < i;j++) {
    if (NULL != (p = strstr(MQTTbuff + j, iot_topic))) {
      memcpy(buf, p + strlen(iot_topic), maxlen + 30);
      return true;
    }
  }
  return false;
}

bool DFRobot_SIM7070G::mqttDisconnect(void)
{
  if (checkSendCmd("AT+CACLOSE=0\r\n", "OK")) {
    return true;
  } else {
    return false;
  }
}

bool DFRobot_SIM7070G::httpInit(eNet net)
{
  if (net == eNB) {
    // if(!checkSendCmd("AT+SAPBR=3,1,\"APN\",\"ctnb\"\r\n","OK")){
    //   return false;
    // }
  } else if (net == eGPRS) {
    // if(!checkSendCmd("AT+SAPBR=3,1,\"APN\",\"cmnet\"\r\n","OK")){
    //   return false;
    // }
  }
  if (!checkSendCmd("AT+CLTS=1\r\n", "OK")) {   // Get local timestamp   Time synchronization
    return false;
  }
  return true;
}

bool DFRobot_SIM7070G::httpConnect(const char* host)
{
  httpDisconnect();
  delay(1000);
  sendCmd("AT+SHCONF=\"URL\",\"");
  sendCmd(host);
  if (!checkSendCmd("\"\r\n", "OK", 100)) {
    return false;
  }
  if (!checkSendCmd("AT+SHCONF=\"BODYLEN\",1024\r\n", "OK", 100)) {
    return false;
  }
  if (!checkSendCmd("AT+SHCONF=\"HEADERLEN\",350\r\n", "OK", 100)) {
    return false;
  }
  if (!checkSendCmd("AT+SHCONN\r\n", "OK", 2000)) {
    return false;
  }
  return true;
}

bool DFRobot_SIM7070G::httpPost(const char* host, String data)
{

  if (!httpConnect(host)) {
    return false;
  }
  char resp[40];
  sendCmd("AT+SHBOD="); 
  itoa(data.length(), resp, 10);
  sendString(resp);
  sendCmd(",1000\r\n");

  delay(100);
  DBG(data.c_str());
  sendString(data.c_str());

  while (1) {
    readBuffer(resp, 20);
    if (NULL != strstr(resp, "OK")) {
      break;
    }
    if (NULL != strstr(resp, "ERROR")) {
      return false;
    }
  }

  sendCmd("AT+SHREQ=\"/post\",3\r\n");
  while (1) {
    readBuffer(resp, 40);
    if (NULL != strstr(resp, "200")) {
      break;
    }
    if (NULL != strstr(resp, "601")) {
      return false;
    }
  }

  sendCmd("AT+SHREAD=0,");
  sendCmd(strrchr(resp, ',') + 1);
  sendCmd("\r\n");
  get_String();
  return true;
}

void DFRobot_SIM7070G::httpGet(const char* host)
{
  char resp[40];
  if (!httpConnect(host)) {
    return;
  }
  sendCmd("AT+SHREQ=\"/get\",1\r\n");
  while (1) {
    readBuffer(resp, 40);
    if (NULL != strstr(resp, "200")) {
      break;
    }
    if (NULL != strstr(resp, "601")) {
      return;
    }
  }

  sendCmd("AT+SHREAD=0,");
  sendCmd(strrchr(resp, ',') + 1);
  sendCmd("\r\n");
  get_String();
}

void DFRobot_SIM7070G::httpDisconnect(void)
{
  checkSendCmd("AT+SHDISC\r\n", "OK", 100);
}

int DFRobot_SIM7070G::recv(char* buf, int maxlen)
{
  uint16_t bufLen = maxlen + 50;
  char dataBuffer[bufLen] = { "\0" };
  readBuffer(dataBuffer, bufLen, 100);

  char num[8] = { "\0" };
  itoa(maxlen, num, 10);
  sendCmd("AT+CARECV=0,");
  sendCmd(num);
  sendCmd("\r\n");
  uint64_t timecnt = millis();
  while (1) {
    if (checkReadable()) {
      cleanBuffer(dataBuffer, bufLen);
      readBuffer(dataBuffer, bufLen);
      const char* carecvStart = strstr(dataBuffer, "+CARECV:");
      if (NULL != carecvStart) {
        int dataLength = 0;
        carecvStart += strlen("+CARECV: ");
        while (*carecvStart >= '0' && *carecvStart <= '9') {
          dataLength = dataLength * 10 + (*carecvStart - '0');
          carecvStart++;
        }
        if (*carecvStart == ',') {
          carecvStart++;
        }
        memcpy(buf, carecvStart, dataLength);
        return dataLength;
      } else {
        return 0;
      }
    }
    if ((millis() - timecnt) > 2000) {
      break;
    }
  }
}

bool DFRobot_SIM7070G::getPosition(void)
{
  char posBuffer[150];
  char* position;
  cleanBuffer(posBuffer, 150);
  sendCmd("AT+CGNSINF\r\n");
  readBuffer(posBuffer, 150);
  DBG(posBuffer);
  if (NULL != strstr(posBuffer, "+CGNSINF: 1,1")) {
    setCommandCounter(4);
  } else {
    return false;
  }
  if (getCommandCounter() == 4) {
    position = strstr(posBuffer, ".000");
    memcpy(_latitude, position + 5, 7);
    memcpy(_longitude, position + 15, 7);
    setCommandCounter(5);
    return true;
  } else {
    return false;
  }
}

const char* DFRobot_SIM7070G::getLatitude(void)
{
  if (getCommandCounter() >= 5) {
    setCommandCounter(6);
    return _latitude;
  } else {
    return "error";
  }
}

const char* DFRobot_SIM7070G::getLongitude(void)
{
  if (getCommandCounter() >= 5) {
    setCommandCounter(6);
    return _longitude;
  } else {
    return "error";
  }
}

bool DFRobot_SIM7070G::closeNetwork(void)
{
  if (checkSendCmd("AT+CACLOSE=0\r\n", "OK")) {
    return true;
  } else {
    return false;
  }
}

int DFRobot_SIM7070G::batteryPower(void)
{
  char batteryBuff[26];
  char* pBattery;
  int i = 0, j = 0, k = 0;
  cleanBuffer(batteryBuff, 26);
  sendCmd("AT+CBC\r\n");
  readBuffer(batteryBuff, 26);
  pBattery = strstr(batteryBuff, "+CBC:");
  i = *(pBattery + 8) - 48;
  j = *(pBattery + 9) - 48;
  k = (i * 10) + j;
  return  k;
}