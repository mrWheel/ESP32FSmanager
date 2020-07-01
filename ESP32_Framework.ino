//
//  Opzet voor een ESP32_Framework
//
//  This is "Work in Progress"!!
//
//================================================================================
// There is a problem in the WebServer library!!!
//
// https://github.com/espressif/arduino-esp32/issues/3652#issuecomment-617058198
//
// -- changed WebServer.cpp in
// ~/Library/Arduino15/packages/esp32/hardware/esp32/1.0.4/libraries/WebServer/src
//
//--------------------------------------------------------------------------
//316  if (_currentClient.available()) {
//       if (_parseRequest(_currentClient)) {
//         // because HTTP_MAX_SEND_WAIT is expressed in milliseconds,
//         // it must be divided by 1000
//         _currentClient.setTimeout(HTTP_MAX_SEND_WAIT / 1000);
//         _contentLength = CONTENT_LENGTH_NOT_SET;
//         _handleRequest();
//
//         /* this comment out for chrome/maxthon browsers
//         if (_currentClient.connected()) {
//           _currentStatus = HC_WAIT_CLOSE;
//           _statusChange = millis();
//           keepCurrentClient = true;
//         } 
//         */
//       }
//     } else { // !_currentClient.available()
//     ......
//--------------------------------------------------------------------------
// original file in ../srcORG/
//================================================================================

#define _FW_VERSION "v0.1.0 01-07-2020"

#define _HOSTNAME   "ESP32_Framework"

#include "ESP32_Framework.h"
#include "framework.h"


//====================================================================
void setup() 
{
  //-- setup serial port --
  Serial.begin(115200);
  while(!Serial) { /* wait a bit */ }

  //-- save last reset reason --
  translate_reset_reason(lastReset, sizeof(lastReset), (int)esp_reset_reason());
  DebugTf("Last reset reason[%d] => [%s]\r\n\n", esp_reset_reason(), lastReset);

  //-- set pinModes for GPIO ports --
  pinMode(LED_BUILTIN, OUTPUT);

  //-- report starting firmware --
  DebugTln("\r\n[ESP32_Framework]\r\n");
  DebugTf("Booting....[%s]\r\n\r\n", String(_FW_VERSION).c_str());

  //-- setup framework --
  frameworkSetup();

  //-- from here on do firmware specific stuff --
  
} // setup()


//====================================================================
void loop() 
{
  // this one is needed!!
  frameworkLoop();
  
  // eat your hart out!
  if (millis() > blinkyTimer)
  {
    blinkyTimer = millis() + 2000;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  if (millis() > lastStatusTimer)
  {
    lastStatusTimer = millis() + 600000;
    readLastStatus(); // 
    DebugFlush();
    DebugTf("number of reBoots [%u]\r\n", nrReboots);
  }
  
} // loop()

/* eof */
