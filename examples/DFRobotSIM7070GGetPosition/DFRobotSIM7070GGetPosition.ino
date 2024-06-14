/*!
 * @file    DFRobot_SIM7070G_getPosition.ino
 * @n       brief description : SIM7070G needs 7-12V DC power supply
 * @brief   brief description : This example use for get longitude and latitude
 * @n       With initialization completed, we use getPosition() to positioning current position
 * @n       Use getLongitude() to get longitude(-180,180), negative is west positive is east
 * @n       Use getLatitude() to get latitude(-90,90), negative is south positive is north
 * @n       If you use Mega please connect PIN8 PIN10 and set PIN_RX = 10
 *
 * @n       The positioning function only available in outdoor
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author      [qsjhyy](yihuan.huang@dfrobot.com)
 * @version     V1.0.0
 * @date        2024-06-06
 * @url         https://github.com/DFRobot/DFRobot_SIM7070G
 */

#include <DFRobot_SIM7070G.h>

#define PIN_TX     7
#define PIN_RX     8
SoftwareSerial     mySerial(PIN_RX, PIN_TX);
DFRobot_SIM7070G    sim7070g(&mySerial);

void setup()
{
  delay(1500);   // UNO reset delay
  Serial.begin(115200);
  mySerial.begin(19200);

  Serial.println("Turn ON SIM7070G......");
  if (sim7070g.turnON()) {                                          //Turn ON SIM7070G
    Serial.println("Turn ON !");
  }

  Serial.println("Set baud rate......");
  while (1) {
    if (sim7070g.setBaudRate(19200)) {                            //Set SIM7070G baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
      Serial.println("Set baud rate:19200");
      break;
    } else {
      Serial.println("Faile to set baud rate");
      delay(1000);
    }
  }

  Serial.println("Check SIM card......");
  if (sim7070g.checkSIMStatus()) {                                  //Check SIM card
    Serial.println("SIM card READY");
  } else {
    Serial.println("SIM card ERROR, Check if you have insert SIM card and restart SIM7070G");
    while (1);
  }

  Serial.println("Init positioning function......");
  while (1) {
    if (sim7070g.initPos()) {
      Serial.println("Positioning function initialized");
      break;
    } else {
      Serial.println("Fail to init positioning function");
      delay(1000);
    }
  }
}

void loop()
{
  Serial.println("Enter anything end with CRLF to get positioning ");
  char loge[10];
  readSerial(loge);
  Serial.println("Getting position......");
  if (sim7070g.getPosition()) {                                     //Get the current position
    Serial.print(" Longitude : ");
    Serial.println(sim7070g.getLongitude());                    //Get longitude
    Serial.print(" Latitude : ");
    Serial.println(sim7070g.getLatitude());                     //Get latitude
  } else {
    Serial.println("Wrong data try again");
  }
}

int readSerial(char result[])
{
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}
