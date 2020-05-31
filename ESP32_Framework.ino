//
//  Opzet voor een ESP32_Framework
//
//  This is "Work in Progress"!!
//

#define _FW_VERSION "v0.0.7 29-05-2020"

#define _HOSTNAME   "ESP32framework"
#include "ESP32_Framework.h"

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
  DebugTf("values timestamp[%s], nrReboots[%u], dummy[%s]\r\n"
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
  DebugTf("writeLastStatus() => %s; %u;\r\n", timeStamp, nrReboots);

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


//====================================================================
void setup() 
{
  Serial.begin(115200);
  while(!Serial) { /* wait a bit */ }

  lastReset     = ((String)esp_reset_reason()).c_str();

  pinMode(LED_BUILTIN, OUTPUT);
  
  DebugTln("\r\n[ESP32_Framework]\r\n");
  DebugTf("Booting....[%s]\r\n\r\n", String(_FW_VERSION).c_str());
      
  //================ SPIFFS ===========================================
  // https://github.com/espressif/arduino-esp32/issues/3000
  if (SPIFFS.begin(false,"/spiffs",30)) 
  //if (SPIFFS.begin()) 
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
  startWiFi(_HOSTNAME, 240);  // timeout 4 minuten
  digitalWrite(LED_BUILTIN, LOW);

  startTelnet();
  DebugT("\nGebruik 'telnet ");
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

  snprintf(cMsg, sizeof(cMsg), "Last reset reason: [%s]\r", ((String)esp_reset_reason()).c_str());
  DebugTln(cMsg);

  //==========================================================//
  // writeLastStatus();  // only for firsttime initialization //
  //==========================================================//
  readLastStatus(); // 
  DebugTf("nrReboots[%u]\r\n\n", nrReboots++);                                                                    
  writeLastStatus();

//================ Start HTTP Server ================================
  setupFSexplorer();
  updateServer.setIndexPage(updateServerIndex);
  updateServer.setSuccessPage(updateServerSuccess);
  setupUpdateServer(&httpServer, "/update");
  
  httpServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    sendIndexPage(request);
  });

  httpServer.on("/api", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    restAPI(request, 0, 0);
  });
  httpServer.on("/api", HTTP_POST, [](AsyncWebServerRequest * request){},
    NULL,
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
      restAPI(request, data, len);
      //request->send(200);
  });
  httpServer.serveStatic("/index.css",      SPIFFS, "/index.css");
  httpServer.serveStatic("/index.js",       SPIFFS, "/index.js");
  httpServer.serveStatic("/FSexplorer.png", SPIFFS, "/FSexplorer.png");
  httpServer.serveStatic("/settings.png",   SPIFFS, "/settings.png");
  httpServer.serveStatic("/favicon.ico",    SPIFFS, "/favicon.ico");

//--httpServer.onNotFound(notFound);  // defined in FSexplorer

  httpServer.begin();
  DebugTln("\r\nServer started\r");
  
} // setup()

//====================================================================
void loop() 
{
  // eat your hart out!
  if (millis() > blinkyTimer)
  {
    blinkyTimer = millis() + 2000;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  if (millis() > lastStatusTimer)
  {
    lastStatusTimer = millis() + 60000;
    DebugFlush();
    readLastStatus(); // 
    DebugFlush();
    DebugTf("number of restarts [%u]\r\n", nrReboots);
    DebugFlush();
  }

  if (doFormatSPIFFS)
  {
    doFormatSPIFFS = false;
    DebugT(F("Format SPIFFS .."));
    SPIFFS.format();
    Debugln(" Done!");
  }
} // loop()
