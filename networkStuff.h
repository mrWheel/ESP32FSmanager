/*
***************************************************************************  
**  Program : networkStuff.h
**  Version : 2.0  10-05-2020
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
**      Usage:
**      
**      #define HOSTNAME  thisProject      
**      
**      setup()
**      {
**        startTelnet();
**        startWiFi(_HOSTNAME, 240);  // timeout 4 minuten
**        startMDNS(_HOSTNAME);
**        httpServer.on("/index",     <sendIndexPage>);
**        httpServer.on("/index.html",<sendIndexPage>);
**        httpServer.begin();
**      }
**      
**      loop()
**      {
**        handleWiFi();
**        MDNS.update();
**        httpServer.handleClient();
**        .
**        .
**      }
*/


  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <ESPmDNS.h>
  #include <WiFiUdp.h>              // part of ESP32 Core
//#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager/releases 2.0.1-alpha
  #include <ESPAsyncWiFiManager.h>  // https://github.com/alanswx/ESPAsyncWiFiManager
  #include <FS.h>
  //#include <SPIFFS.h>

/**** UITZOEKEN ****/
  #include "ESPModUpdateServer.h"  // <<modified version of ESP32ModUpdateServer.h by Robert>>
  #include "UpdateServerHtml.h"   


  // WiFi Server object and parameters
  AsyncWebServer      httpServer(80);
  ESPModUpdateServer  updateServer;
  DNSServer           dns;

bool        SPIFFSmounted; 
bool        isConnected = false;

//gets called when WiFiManager enters configuration mode
//===========================================================================================
void configModeCallback (AsyncWiFiManager *myWiFiManager) 
{
  //char cMsg[100];
  DebugTln(F("Entered config mode\r"));
  DebugTln(WiFi.softAPIP().toString());
  //if you used auto generated SSID, print it
  //snprintf(cMsg, sizeof(cMsg), "Connect to AP '%s' and configure WiFi on  192.168.4.1   ", _HOSTNAME);
  //DebugTln(cMsg);
  DebugTln(myWiFiManager->getConfigPortalSSID());

} // configModeCallback()


//===========================================================================================
void startWiFi(const char* hostname, int timeOut) 
{
//WiFiManager manageWiFi;
  AsyncWiFiManager manageWiFi(&httpServer,&dns);
  uint32_t lTime = millis();
  String thisAP = String(hostname) + "-" + WiFi.macAddress();

  DebugTln("start ...");
  
  manageWiFi.setDebugOutput(true);

  //reset saved settings ==>> LIJKT NIET TE WERKEN!!
  // ==> change WiFi.disconnect(true) to: WiFi.disconnect(true, true); in lib.
  //manageWiFi.resetSettings();
  
  //--- set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  //manageWiFi.setAPCallback(configModeCallback);

  //--- sets timeout until configuration portal gets turned off
  //--- useful to make it all retry or go to sleep in seconds
  //manageWiFi.setTimeout(240);  // 4 minuten
  manageWiFi.setTimeout(timeOut);  // in seconden ...
  
  //--- fetches ssid and pass and tries to connect
  //--- if it does not connect it starts an access point with the specified name
  //--- here  "ESP32_Framework-<MAC>"
  //--- and goes into a blocking loop awaiting configuration
  if (!manageWiFi.autoConnect(thisAP.c_str())) 
  {
    DebugTln(F("failed to connect and hit timeout"));

    //reset and try again, or maybe put it to deep sleep
    //delay(3000);
    //ESP.reset();
    //delay(2000);
    DebugTf(" took [%d] seconds ==> ERROR!\r\n", (millis() - lTime) / 1000);
    return;
  }
  
  //WiFi.setSleep(false);// <--- this command disables WiFi energy save mode and eliminate connected():
  
  Debugln();
  DebugT(F("Connected to " )); Debugln (WiFi.SSID());
  DebugT(F("IP address: " ));  Debugln (WiFi.localIP());
  DebugT(F("IP gateway: " ));  Debugln (WiFi.gatewayIP());
  Debugln();

/**** UITZOEKEN ***/
/********
  httpUpdater.setup(&httpServer);
  httpUpdater.setIndexPage(UpdateServerIndex);
  httpUpdater.setSuccessPage(UpdateServerSuccess);
********/
  DebugTf(" took [%d] seconds => OK!\r\n", (millis() - lTime) / 1000);
  
} // startWiFi()


//===========================================================================================
void startTelnet() 
{
  TelnetStream.begin();
  DebugTln(F("\nTelnet server started .."));
  TelnetStream.flush();

} // startTelnet()


//=======================================================================
void startMDNS(const char *Hostname) 
{
  DebugTf("[1] mDNS setup as [%s.local]\r\n", Hostname);
  if (MDNS.begin(Hostname))               // Start the mDNS responder for Hostname.local
  {
    DebugTf("[2] mDNS responder started as [%s.local]\r\n", Hostname);
  } 
  else 
  {
    DebugTln(F("[3] Error setting up MDNS responder!\r\n"));
  }
  MDNS.addService("http", "tcp", 80);
  
} // startMDNS()


//====================================================================
boolean setupUpdateServer(AsyncWebServer *server, const char* path) 
{
  server->on(path, HTTP_GET, [](AsyncWebServerRequest *request)
  {
    updateServer.handleUpdate(request);
  });
  server->on("/doUpdate", HTTP_POST,
    [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
                  size_t len, bool final) 
  {
    updateServer.handleDoUpdate(request, filename, index, data, len, final);
  });
  //server->onNotFound([](AsyncWebServerRequest *request)
  //{
  //  request->send(404);
  //});
  
  //server->begin();
#ifdef ESP32
  //Update.onProgress(printProgress);
#endif

} // setupUpdateServer()

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
