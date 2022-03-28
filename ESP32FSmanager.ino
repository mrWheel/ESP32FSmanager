/*
***************************************************************************  
 *  Program  : ESP32FSmanager - template- 
 *  Copyright (c) 2021 - 2022 Willem Aandewiel
 */
#define _FW_VERSION "v0.0.0 (dd-mm-eeyy)"
/* 
*  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************  
 * Board:         "Generic ESP32 Module"  // always! even if you have a Wemos or NodeMCU!
 * Builtin Led:   "2"
 * CPU Frequency: "80 MHz" or "160 MHz"
 * Flash Size:    "4MB (FS:2MB OTA:~???KB)"
***************************************************************************/  

#define USE_UPDATE_SERVER

//---------- no need to change enything after this ------

#define _HOSTNAME   "ESP32FSmanager"

#define _MAX_LITTLEFS_FILES   30

#include "WiFicredentials.h"
const char ssid[] = WIFI_SSID;    
const char pass[] = WIFI_PASSWORD;  

#include <WiFi.h>
#include <TelnetStream.h>
#include "Debug.h"
#include <LittleFS.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <time.h>
#include "time_zones.h"
const char *ntpServer  = "pool.ntp.org";
const char *TzLocation = "Europe/Amsterdam";
//const char *tz = "CET-1CEST,M3.5.0,M10.5.0/3";
//const long  gmtOffset_sec = 0;
//const int   daylightOffset_sec = 3600;

#ifdef USE_UPDATE_SERVER
//  #include <ModUpdateServer.h>   // https://github.com/mrWheel/ModUpdateServer
  #include "updateServerHtml.h"
  #include "ESP32ModUpdateServer.h"
#endif

#define _LED_PIN        2   // OnBoard (blue) LED

WebServer  httpServer(80);
#ifdef USE_UPDATE_SERVER
  ESP32HTTPUpdateServer httpUpdater(true);
#endif

uint32_t printTimeTimer = 0, pulseTimer = 0;


//------------------------------------------------------
void setup() 
{
  Serial.begin(115200);
  while(!Serial) { delay(10); }
  Serial.println();
  Serial.printf("\r\n[%s]\r\n", _HOSTNAME);
  Serial.flush();

  pinMode(_LED_PIN, OUTPUT);
  
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    Serial.println("WiFi failed! ..Retry to connect.");
    delay(1000);
    WiFi.begin(ssid, pass);
  }

  IPAddress ip = WiFi.localIP();
  DebugTln();
  DebugTln("Connected to WiFi network.");
  DebugT("Connect with Telnet client to ");
  Debugln(ip);
  
  //DebugTf("getTzByLocation(%s)\r\n", getTzByLocation(TzLocation).c_str());
  //DebugTf("NTP server [%s]\r\n", ntpServer);

  //-- 0, 0 because we will use TZ in the next line
  configTime(0, 0, ntpServer);
  //-- Set environment variable with your time zone
  setenv("TZ", getTzByLocation(TzLocation).c_str(), 1);
  tzset();
  printLocalTime();

  DebugTf("[1] mDNS setup as [%s.local]\r\n", _HOSTNAME);
  if (MDNS.begin(_HOSTNAME))               // Start the mDNS responder for Hostname.local
  {
    DebugTf("[2] mDNS responder started as [%s.local]\r\n", _HOSTNAME);
  } 
  else 
  {
    DebugTln(F("[3] Error setting up MDNS responder!\r\n"));
  }
  MDNS.addService("http", "tcp", 80);

  LittleFS.begin();
  
  TelnetStream.begin();

  httpServer.serveStatic("/",               LittleFS, "/index.html");
  httpServer.serveStatic("/index",          LittleFS, "/index.html");
  httpServer.serveStatic("/index.html",     LittleFS, "/index.html");
  httpServer.serveStatic("/index.css",      LittleFS, "/index.css");
  httpServer.serveStatic("/index.js",       LittleFS, "/index.js");

  setupFSmanager();
  
  httpServer.begin();
  
  DebugTln("HTTP httpServer started");

}   // setup()


//------------------------------------------------------
void loop() 
{
  httpServer.handleClient();

  //--- read key's from monitor and telnet
  if (Serial.available()) 
  {
    char cIn = Serial.read();
    if ( (cIn >= ' ' && cIn <= 'z') || (cIn == '\n') ) 
    {
      TelnetStream.print(cIn);
    }
  }
  if (TelnetStream.available()) 
  {
    char cIn = TelnetStream.read();
    if ( (cIn >= ' ' && cIn <= 'z') || (cIn == '\n') ) 
    {
      Serial.print(cIn);
    }
  }

  if ((millis() - pulseTimer) > 2000)
  {
    pulseTimer = millis();
    digitalWrite(_LED_PIN, !digitalRead(_LED_PIN));
  }

  if ((millis() - printTimeTimer) > 60000)
  {
    printTimeTimer = millis();
    printLocalTime();
  }

}   // loop()

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
