/*!
 * @file     DFRobot_SIM7070G_ATtest.ino
 * @n        SIM7070G needs 7-12V DC power supply
 * @brief    This example use the serial port to send AT command to control the SIM7070G
 * @n        With initialization completed, we can enter AT command to SIM7070G directly
 * @n        Common AT commands :
 * @n        AT+CPIN?     : Check SIM card
 * @n        AT+CSQ       : Check signal quality
 * @n        AT+CGATT?    : Check net attached state
 * @n        AT+CNACT=0,1 : Activating network bearer
 * @n        AT+CNACT?    : Get local IP
 *
 * @n        Get the AT command table in Resource folder :
 * @n          SIM7070_SIM7080_SIM7090 Series_AT Command Manual_V1.07.pdf
 * @n        If you use Mega please connect PIN8 PIN10 and set PIN_RX = 10
 * @n        The AT command must end with CRLF
 *
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
  Serial.begin(19200);
  mySerial.begin(19200);

  Serial.println("Turn ON SIM7070G......");
  if (sim7070g.turnON()) {                             //Turn ON SIM7070G
    Serial.println("Turn ON !");
  }

  Serial.println("Set baud rate......");
  while (1) {
    if (sim7070g.setBaudRate(19200)) {               //Set SIM7070G baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
      Serial.println("Set baud rate:19200");
      break;
    } else {
      Serial.println("Faile to set baud rate");
      delay(1000);
    }
  }

  Serial.println("For example, if you type AT\\r\\n, OK\\r\\n will be responsed!");
  Serial.println("Enter your AT command :");
}

void loop()
{
  mySerial.listen();
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  mySerial.flush();
  while (Serial.available()) {
    mySerial.write(Serial.read());
  }
}