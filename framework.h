/* 
***************************************************************************  
**  Program  : ESP32_Framework.h
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

#define SETTINGS_FILE   "/settings.ini"
#define CMSG_SIZE        512
#define JSON_BUFF_MAX   1024
#define LED_BUILTIN        2     // GPIO-02
#define LED_ON          HIGH
#define LED_OFF          LOW

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>              // part of ESP32 Core
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager/releases 2.0.1-alpha
#include <ezTime.h>             // https://github.com/ropg/ezTime v0.8.3
#include <TelnetStream.h>       // https://github.com/jandrassy/TelnetStream
#include <FS.h>
#include <SPIFFS.h>

#include "Debug.h"
#include "espModUpdateServer.h"  
#include "espUpdateServerHtml.h"   

ESP32HTTPUpdateServer updateServer(true);

// WiFi Server object and parameters
WebServer           httpServer(80);
DNSServer           dns;

bool      Verbose = false;
bool      doFormatSPIFFS = false;
char      cMsg[CMSG_SIZE];
char      fChar[10];
char      lastReset[50];
char      settingHostname[41];
uint32_t  nrReboots = 0;
Timezone  CET;
bool      SPIFFSmounted; 
bool      isConnected = false;

const char *flashMode[]    { "QIO", "QOUT", "DIO", "DOUT", "FAST READ", "SLOWREAD", "Unknown" };



/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
****************************************************************************
*/
