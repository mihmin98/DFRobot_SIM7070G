/*!
 * @file   DFRobot_SIM7070G_Upload.ino
 * @n      SIM7070G needs 7-12V DC power supply
 * @brief  This example use SIM7070G to get position then upload the data to iot with MQTT
 * @n      This example needs SIM card
 * @n      The tracker function only available in outdoor
 * @n      If you use Mega please connect PIN8 PIN10 and set PIN_RX = 10
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author      [qsjhyy](yihuan.huang@dfrobot.com)
 * @version     V1.0.0
 * @date        2024-06-06
 * @url         https://github.com/DFRobot/DFRobot_SIM7070G
 */

#include <DFRobot_SIM7070G.h>

 //Login website (http://iot.dfrobot.com.cn) to register an account ,fill the following information based on your account
#define serverIP        "iot.dfrobot.com.cn"
#define IOT_USERNAME    " USER   NAME "
#define IOT_KEY         " PASSWORD    "
#define IOT_TOPIC       " TOPIC       "
#define IOT_CLIENT      "dfrobot"

#define PIN_TX          7
#define PIN_RX          8

SoftwareSerial          mySerial(PIN_RX, PIN_TX);
DFRobot_SIM7070G         sim7070g(&mySerial);

void setup()
{
  delay(1500);   // UNO reset delay
  Serial.begin(115200);
  mySerial.begin(19200);

  Serial.println("Turn ON SIM7070G......");
  if (sim7070g.turnON()) {                                        //Turn ON SIM7070G
    Serial.println("Turn ON !");
  }

  Serial.println("Set baud rate......");
  while (1) {
    if (sim7070g.setBaudRate(19200)) {                          //Set SIM7070G baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
      Serial.println("Set baud rate:19200");
      break;
    } else {
      Serial.println("Faile to set baud rate");
      delay(1000);
    }
  }

  Serial.println("Check SIM card......");
  if (sim7070g.checkSIMStatus()) {                                //Check SIM card
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

  Serial.println("Set net mode......");
  while (1) {
    if (sim7070g.setNetMode(sim7070g.eGPRS)) {                              //Set GPRS mode
      Serial.println("Set GPRS mode");
      break;
    } else {
      Serial.println("Fail to set mode");
      delay(1000);
    }
  }

  Serial.println("Get signal quality......");
  delay(1500);
  int  signalStrength = sim7070g.checkSignalQuality();            //Check signal quality from (0-30)
  Serial.print("signalStrength =");
  Serial.println(signalStrength);
  delay(500);

  Serial.println("Attaching service......");
  while (1) {
    if (sim7070g.attacthService()) {                            //Open the connection
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
  if (sim7070g.getPosition()) {                                   //Get the current position
    Serial.print("Longtude :");
    Serial.println(sim7070g.getLongitude());                  //Get longitude
    Serial.print("Latitude :");
    Serial.println(sim7070g.getLatitude());                   //Get latitude
  } else {
    Serial.println("Wrong data try again");
    return;
  }

  String  sim7070gBuffer;
  sim7070gBuffer = String();
  sim7070gBuffer += " Longitude : ";
  sim7070gBuffer += sim7070g.getLongitude();
  sim7070gBuffer += " Latitude  : ";
  sim7070gBuffer += sim7070g.getLatitude();

  Serial.print("Connect to :");
  Serial.println(serverIP);
  if (sim7070g.openNetwork(sim7070g.eTCP, serverIP, 1883)) {                  //Connect to server
    Serial.println("Connected !");
  } else {
    Serial.println("Failed to connect");
    return;
  }
  delay(200);

  Serial.print("Connect to : ");
  Serial.println(IOT_USERNAME);
  if (sim7070g.mqttConnect(IOT_CLIENT, IOT_USERNAME, IOT_KEY)) {    //MQTT connect request
    Serial.println("Connected !");
  } else {
    Serial.println("Failed to connect");
    return;
  }
  delay(200);

  if (sim7070g.mqttPublish(IOT_TOPIC, sim7070gBuffer)) {            //Send data to topic
    Serial.println("Send OK");
  } else {
    Serial.println("Failed to send");
    return;
  }
  delay(200);

  Serial.println("Close connection......");
  if (sim7070g.closeNetwork()) {                                  //Close connection
    Serial.println("Close connection !");
  } else {
    Serial.println("Fail to close connection !");
    return;
  }
}