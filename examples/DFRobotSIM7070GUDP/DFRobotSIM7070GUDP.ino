/*!
 * @file   DFRobot_SIM7070G_UDP.ino
 * @n      SIM7070G needs 7-12V DC power supply
 * @brief  This example verify UDP connection over a test server
 * @n      With initialization completed, we use connect(UDP,"112.74.93.163",5232) to set UDP connection
 * @n      Then enter data send to server and will receive the data same as entered
 * @n      Thus we finished the UDP send and receive verification
 * @n      If you use Mega please connect PIN8 PIN10 and set PIN_RX = 10
 * @n     The server in this example is provided by the chip supplier for UDP test
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author      [qsjhyy](yihuan.huang@dfrobot.com)
 * @version     V1.0.0
 * @date        2024-06-06
 * @url         https://github.com/DFRobot/DFRobot_SIM7070G
 */

#include <DFRobot_SIM7070G.h>

#define serverIP    "112.74.93.163"
#define serverPort   5232
#define PIN_TX       7
#define PIN_RX       8

SoftwareSerial       mySerial(PIN_RX, PIN_TX);
DFRobot_SIM7070G         sim7070g(&mySerial);
char                 sendData[100];
char                 buff[100];

void setup()
{
  delay(1500);   // UNO reset delay
  int signalStrength;
  Serial.begin(115200);
  mySerial.begin(19200);

  Serial.println("Turn ON SIM7070G......");
  if (sim7070g.turnON()) {                                       //Turn ON SIM7070G
    Serial.println("Turn ON !");
  }

  Serial.println("Set baud rate......");
  while (1) {
    if (sim7070g.setBaudRate(19200)) {                         //Set SIM7070G baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
      Serial.println("Set baud rate:19200");
      break;
    } else {
      Serial.println("Faile to set baud rate");
      delay(1000);
    }
  }

  Serial.println("Check SIM card......");
  if (sim7070g.checkSIMStatus()) {                               //Check SIM card
    Serial.println("SIM card READY");
  } else {
    Serial.println("SIM card ERROR, Check if you have insert SIM card and restart SIM7070G");
    while (1);
  }

  Serial.println("Set net mode......");
  while (1) {
    if (sim7070g.setNetMode(sim7070g.eGPRS)) {                             //Set GPRS mode
      Serial.println("Set GPRS mode");
      break;
    } else {
      Serial.println("Fail to set mode");
      delay(1000);
    }
  }

  Serial.println("Get signal quality......");
  delay(1500);
  signalStrength = sim7070g.checkSignalQuality();                //Check signal quality from (0-30)
  Serial.print("signalStrength =");
  Serial.println(signalStrength);
  delay(500);

  Serial.println("Attaching service......");
  while (1) {
    if (sim7070g.attacthService()) {                           //Open the connection
      Serial.println("Attach service");
      break;
    } else {
      Serial.println("Fail to Attach service");
      delay(1000);
    }
  }
}

void loop()
{
  delay(2000);
  Serial.println("Connecting......");
  if (sim7070g.openNetwork(sim7070g.eUDP, serverIP, serverPort)) {           //Start up UDP connection
    Serial.println("Connect OK");
  } else {
    Serial.println("Fail to connect");
    return;
  }

  Serial.println("Input data :");
  readSerial(sendData);
  Serial.println("Send data :");
  Serial.println(sendData);
  if (sim7070g.send(sendData)) {                                 //Send data to server
    sim7070g.recv(buff, 350);
    Serial.println("Send data, recive :");
    Serial.println(buff);
  } else {
    Serial.println("Failed to send");
  }
  delay(500);

  Serial.println("Close connection......");
  if (sim7070g.closeNetwork()) {                                 //End the connection
    Serial.println("Closed !");
  } else {
    Serial.println("Fail to close connection");
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