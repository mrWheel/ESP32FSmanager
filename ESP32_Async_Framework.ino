//
//  Opzet voor eenESP32_Framework
//
//  This is "Work in Progress"!!
//

#define _FW_VERSION "v0.0.1 27-05-2020"

#define _HOSTNAME   "ESP32framework"
#include "ESP32_Framework.h"

void setup() {
  Serial.begin(115200);
  while(!Serial) { /* wait a bit */ }

  lastReset     = ((String)esp_reset_reason()).c_str();

  pinMode(LED_BUILTIN, OUTPUT);
  
  DebugTln("\r\n[ESP32_Framework]\r\n");
  DebugTf("Booting....[%s]\r\n\r\n", String(_FW_VERSION).c_str());
      
  //================ SPIFFS ===========================================
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

//================ Start HTTP Server ================================
  setupFSexplorer();

  httpServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send(200, "text/plain", indexPage);
    sendIndexPage(request);
  });

  httpServer.on("/api", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send(200, "text/plain", indexPage);
    restAPI(request);
  });
  httpServer.serveStatic("/index.css", SPIFFS, "/index.css");
  httpServer.serveStatic("/index.js",  SPIFFS, "/index.js");
  httpServer.serveStatic("/FSexplorer.png",  SPIFFS, "/FSexplorer.png");
  httpServer.serveStatic("/settings.png",  SPIFFS, "/settings.png");
  httpServer.serveStatic("/flavicon.ico",  SPIFFS, "/flavicon.ico");

//--httpServer.onNotFound(notFound);  // defined in FSexplorer

  httpServer.begin();
  DebugTln("\nServer started\r");
  
} // setup()

void loop() {
  // eat your hart out!
}
