//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#define _FW_VERSION "v0.0.1 25-05-2020"

#define _HOSTNAME   "ESP32framework"
#include "ESP32_Framework.h"

//void notFound(AsyncWebServerRequest *request) {
//    request->send(404, "text/plain", "Not found");
//}

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
/*****
    // Send a GET request to <IP>/get?message=<message>
    httpServer.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    httpServer.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        DebugTln(request->getParam(PARAM_MESSAGE, true)->value());
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });
***/
  httpServer.serveStatic("/index.css", SPIFFS, "/index.css");
  httpServer.serveStatic("/index.js",  SPIFFS, "/index.js");
  httpServer.serveStatic("/FSexplorer.png",  SPIFFS, "/FSexplorer.png");
  httpServer.serveStatic("/settings.png",  SPIFFS, "/settings.png");
  httpServer.serveStatic("/flavicon.ico",  SPIFFS, "/flavicon.ico");

//--httpServer.onNotFound(notFound);

  httpServer.begin();
  DebugTln("\nServer started\r");
  
} // setup()

void loop() {
}
