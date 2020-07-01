/* 
***************************************************************************  
**  Program  : ESP32_Framework
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//====================================================================
void frameworkSetup() 
{      
  //================ SPIFFS ===========================================
  // https://github.com/espressif/arduino-esp32/issues/3000
  //if (SPIFFS.begin(false,"/spiffs",30)) 
  if (SPIFFS.begin()) 
  {
    DebugTln(F("SPIFFS Mount succesfull\r"));
    SPIFFSmounted = true;
  } else { 
    DebugTln(F("SPIFFS Mount failed\r"));   // Serious problem with SPIFFS 
    SPIFFSmounted = false;
  }
  
  readSettings(true);

  // attempt to connect to Wifi network:
  DebugTln("Attempting to connect to WiFi network\r");
  // Connect to and initialise WiFi network
  digitalWrite(LED_BUILTIN, HIGH);
  //-- depending on the used case 4 minutes (240 sec's) might be a good choice --
  startWiFi(_HOSTNAME, 60);  // timeout 1 minuten
  if (WiFi.status() != WL_CONNECTED)  // 3
  {
    DebugTln("No WiFi ... That's bad!\r");
    DebugTln(".. better restart ..\r\n\n");
    delay(500);
    ESP.restart();
    delay(500);
  }
  digitalWrite(LED_BUILTIN, LOW);

  startTelnet();
  DebugT("Gebruik 'telnet ");
  Debug(WiFi.localIP());
  Debugln("' voor verdere debugging\r\n");

  startMDNS(settingHostname);

  //--- ezTime initialisation
  setDebug(INFO);  
  waitForSync(); 
  CET.setLocation(F("Europe/Amsterdam"));
  CET.setDefault(); 
  
  Debugln("UTC time: "+ UTC.dateTime());
  Debugln("CET time: "+ CET.dateTime());
  setDebug(ERROR);  

  //==========================================================//
  if (esp_reset_reason() == 1 || esp_reset_reason() == 3)
  {
    nrReboots = 0;
    writeLastStatus();  //-- only after compilation or firmware upload //
  }
  //==========================================================//
  readLastStatus(); // 
  nrReboots++;
  DebugTf("nrReboots[%u]\r\n", nrReboots);
  writeLastStatus();

  snprintf(cMsg, sizeof(cMsg), "Last reset reason: [%s]\r", lastReset);
  DebugTln(cMsg);

//================ Start HTTP Server ================================
  setupFSexplorer();
  
  httpServer.serveStatic("/",             SPIFFS, "/index.html");
  httpServer.serveStatic("/index",        SPIFFS, "/index.html");
  httpServer.serveStatic("/index.html",   SPIFFS, "/index.html");
  httpServer.serveStatic("/index.js",     SPIFFS, "/index.js");
  httpServer.serveStatic("/index.css",    SPIFFS, "/index.css");
  httpServer.serveStatic("/settings.png", SPIFFS, "/settings.png");
  httpServer.serveStatic("/favicon.ico",  SPIFFS, "/favicon.ico");
  
  httpServer.on("/api/v0/get/get_1_function",     HTTP_GET,  v0_Get_restAPI);
  httpServer.on("/api/v0/get/get_2_function",  HTTP_GET,  v0_Get_restAPI);
  httpServer.on("/api/v0/get/devtime",      HTTP_GET,  v0_Get_restAPI);
  httpServer.on("/api/v0/get/devinfo",      HTTP_GET,  v0_Get_restAPI);
  
  httpServer.on("/api/v0/post/post_2_function",   HTTP_POST, v0_Post_restAPI);
  httpServer.on("/api/v0/post/post_1_function", HTTP_POST, v0_Post_restAPI);
  
//--httpServer.onNotFound(notFound);  // defined in FSexplorer

  httpServer.begin();
  DebugTln("Server started\r");
  
} // frameworkSetup()


//====================================================================
void frameworkLoop()
{
  events(); // trigger ezTime update etc.
  httpServer.handleClient();
  
  //--- check if WiFi still connected!
  int WiFiStatus = WiFi.status();
  if (WiFiStatus != WL_CONNECTED)  // 3
  {
    DebugTf("WiFiStatus [%d]\r\n", WiFiStatus);
    DebugT("Attempting to re-connect to WiFi network..");
    // Connect to and initialise WiFi network
    digitalWrite(LED_BUILTIN, HIGH);
    WiFi.begin();
    if (WiFi.waitForConnectResult() != WL_CONNECTED) 
    {
      Debugln(".. better restart ..\r\n\n");
      delay(500);
      ESP.restart();
      delay(500);
    }
    Debugln("ok!\r\n");
    digitalWrite(LED_BUILTIN, LOW);
  }
    
  if (doFormatSPIFFS)
  {
    doFormatSPIFFS = false;
    DebugT(F("Format SPIFFS .."));
    SPIFFS.format();
    Debugln(" Done!");
  }


} // frameworkLoop();



//====================================================================
void readLastStatus()
{
  char buffer[50] = "";
  char dummy[50] = "";
  char spiffsTimestamp[21] = "";
  
  File _file = SPIFFS.open("/lastStatus.csv", "r");
  if (!_file)
  {
    DebugTln("read(): No /lastStatus.csv found ..");
  }
  if(_file.available()) {
    int l = _file.readBytesUntil('\n', buffer, sizeof(buffer));
    buffer[l] = 0;
    //DebugTf("read lastUpdate[%s]\r\n", buffer);
    sscanf(buffer, "%[^;]; %u; %[^;]", spiffsTimestamp, &nrReboots, dummy);
    yield();
  }
  _file.close();
  
  strTrim(spiffsTimestamp, sizeof(spiffsTimestamp), ' ');
  DebugTf("timeStamp[%s], nrReboots[%u], dummy[%s]\r\n"
                                                    , spiffsTimestamp
                                                    , nrReboots
                                                    , dummy);
  
}  // readLastStatus()


//====================================================================
void writeLastStatus()
{
  char buffer[50] = "";
  char timeStamp[21];
  
  snprintf(timeStamp, 20, "%04d-%02d-%02d %02d:%02d:%02d", year(), month(), day()
                                                         , hour(), minute(), second());
  DebugTf("timeStamp[%s], nrReboots[%u]\r\n", timeStamp, nrReboots);

  File _file = SPIFFS.open("/lastStatus.csv", "w");
  if (!_file)
  {
    DebugTln("write(): No /lastStatus.csv found ..");
  }
  snprintf(buffer, sizeof(buffer), "%-20.20s; %010u; %s;\n"
                                    , timeStamp
                                            , nrReboots
                                                    , "meta data");
  _file.print(buffer);
  _file.flush();
  _file.close();
  
} // writeLastStatus()
  
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
