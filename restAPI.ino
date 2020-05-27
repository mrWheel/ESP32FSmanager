/* 
***************************************************************************  
**  Program  : restAPI, part of ESP32_Framework
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/


//=======================================================================
void restAPI(AsyncWebServerRequest *request) 
{
  char fName[40] = "";
  char URI[50]   = "";
  String words[10];

  strncpy( URI, request->url().c_str(), sizeof(URI) );

  DebugT("from[");
  Debug(request->client()->remoteIP());
  if (request->method() == HTTP_GET)
  {
    Debugf("] URI[%s] method[GET] \r\n", URI);
  }
  else  
  {
    Debugf("] URI[%s] method[PUT] \r\n", URI); 
  }

  int params = request->params();
  for(int i=0;i<params;i++)
  {
    AsyncWebParameter* p = request->getParam(i);
    if(p->isFile()){
      DebugTf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
    } else if(p->isPost()){
      DebugTf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    } else {
      DebugTf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
  }

  if (ESP.getFreeHeap() < 8500) // to prevent firmware from crashing!
  {
    DebugTf("==> Bailout due to low heap (%d bytes))\r\n", ESP.getFreeHeap() );
    request->send(500, "text/plain", "500: internal httpServer error (low heap)\r\n"); 
    return;
  }

  int8_t wc = splitString(URI, '/', words, 10);
  
  if (Verbose) 
  {
    DebugT(">>");
    for (int w=0; w<wc; w++)
    {
      Debugf("word[%d] => [%s], ", w, words[w].c_str());
    }
    Debugln(" ");
  }

  if (words[1] != "api")
  {
    sendApiNotFound(request, URI);
    return;
  }

  if (words[2] != "v0")
  {
    sendApiNotFound(request, URI);
    return;
  }

  if (words[3] == "devinfo")
  {
    sendDeviceInfo(request);
  }
  else if (words[3] == "devtime")
  {
    sendDeviceTime(request);
  }
  else if (words[3] == "settings")
  {
    if (request->method() == HTTP_PUT || request->method() == HTTP_POST)
    {
      postSettings(request);
    }
    else
    {
      sendDeviceSettings(request);
    }
  }
  else sendApiNotFound(request, URI);
  
} // restAPI()

//=======================================================================
void sendDeviceInfo(AsyncWebServerRequest *request) 
{
  AsyncResponseStream *response = request->beginResponseStream("application/json");

  DebugTln("-");
  DebugFlush();

  sendStartJsonObj(response, "devinfo");

  sendNestedJsonObj(response, "author", "Willem Aandewiel (www.aandewiel.nl)");
  sendNestedJsonObj(response, "fwversion", _FW_VERSION);

  snprintf(cMsg, sizeof(cMsg), "%s %s", __DATE__, __TIME__);
  sendNestedJsonObj(response, "compiled", cMsg);
  sendNestedJsonObj(response, "hostname", settingHostname);
  sendNestedJsonObj(response, "ipaddress", WiFi.localIP().toString().c_str());
  sendNestedJsonObj(response, "macaddress", WiFi.macAddress().c_str());
  sendNestedJsonObj(response, "freeheap", ESP.getFreeHeap());
  sendNestedJsonObj(response, "maxfreeblock", ESP.getMaxAllocHeap());
  sendNestedJsonObj(response, "chipid", String(((uint32_t)ESP.getEfuseMac()), HEX ).c_str());
//sendNestedJsonObj(response, "coreversion", String( ESP.getCoreVersion() ).c_str() );
  sendNestedJsonObj(response, "sdkversion", String( ESP.getSdkVersion() ).c_str());
  sendNestedJsonObj(response, "cpufreq", ESP.getCpuFreqMHz());
  sendNestedJsonObj(response, "sketchsize", formatFloat( (ESP.getSketchSize() / 1024.0), 3));
  sendNestedJsonObj(response, "freesketchspace", formatFloat( (ESP.getFreeSketchSpace() / 1024.0), 3));

//snprintf(cMsg, sizeof(cMsg), "%08X", ESP.getFlashChipId());
//sendNestedJsonObj(response, "flashchipid", cMsg);  // flashChipId
  sendNestedJsonObj(response, "flashchipsize", formatFloat((ESP.getFlashChipSize() / 1024.0 / 1024.0), 3));
//sendNestedJsonObj(response, "flashchiprealsize", formatFloat((ESP.getFlashChipRealSize() / 1024.0 / 1024.0), 3));

  //SPIFFS.info(SPIFFSinfo);
  sendNestedJsonObj(response, "spiffssize", formatFloat((SPIFFS.totalBytes() / (1024.0 * 1024.0)), 0));

  sendNestedJsonObj(response, "flashchipspeed", formatFloat((ESP.getFlashChipSpeed() / 1000.0 / 1000.0), 0));

  FlashMode_t ideMode = ESP.getFlashChipMode();
  //sendNestedJsonObj(response, "flashchipmode", flashMode[ideMode]);
  sendNestedJsonObj(response, "flashchipmode", ESP.getFlashChipMode());
  sendNestedJsonObj(response, "ssid", WiFi.SSID().c_str());
  sendNestedJsonObj(response, "wifirssi", WiFi.RSSI());
//sendNestedJsonObj(response, "uptime", upTime());

  //sendNestedJsonObj(response, "lastreset", lastReset);

  response->print("\r\n]}\r\n");
  request->send(response);

} // sendDeviceInfo()


//=======================================================================
void sendDeviceTime(AsyncWebServerRequest *request) 
{
  char actTime[50];
  AsyncResponseStream *response = request->beginResponseStream("application/json");

  DebugTln("-");
  DebugFlush();
  
  sendStartJsonObj(response, "devtime");
  snprintf(actTime, 49, "%04d-%02d-%02d %02d:%02d:%02d", year(), month(), day()
                                                       , hour(), minute(), second());
  sendNestedJsonObj(response, "dateTime", actTime); 
  sendNestedJsonObj(response, "epoch", (int)now());

  sendEndJsonObj(response);
  request->send(response);

} // sendDeviceTime()


//=======================================================================
void sendDeviceSettings(AsyncWebServerRequest *request) 
{
  AsyncResponseStream *response = request->beginResponseStream("application/json");

  DebugTln("sending device settings ...\r");

  sendStartJsonObj(response, "settings");
  
  sendJsonSettingObj(response, "hostname", settingHostname, "s", sizeof(settingHostname) -1);
//sendJsonSettingObj(response, "float",    settingFloat,    "f", 0, 10,  5);
//sendJsonSettingObj(response, "intager",  settingInteger , "i", 2, 60);

  sendEndJsonObj(response);
  request->send(response);

} // sendDeviceSettings()


//**** UITZOEKEN ****/
//=======================================================================
void postSettings(AsyncWebServerRequest *request)
{
  //------------------------------------------------------------ 
  // json string: {"name":"settingInterval","value":9}  
  // json string: {"name":"settingHostname","value":"abc"}  
  //------------------------------------------------------------ 
  // so, why not use ArduinoJSON library?
  // I say: try it yourself ;-) It won't be easy
      AsyncResponseStream *response = request->beginResponseStream("application/json");
      String wOut[5];
      String wPair[5];
      String jsonIn = "";
      char jsonChar[512] = "";
      int args = request->args();
      for(int i=0;i<args;i++){
          Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
          snprintf(cMsg, sizeof(cMsg), "%s: %s", request->argName(i).c_str(), request->arg(i).c_str());
          strConcat(jsonChar, sizeof(jsonChar), cMsg);
      }
      jsonIn = String(jsonChar);
      char field[25] = "";
      char newValue[101]="";
      jsonIn.replace("{", "");
      jsonIn.replace("}", "");
      jsonIn.replace("\"", "");
      int8_t wp = splitString(jsonIn.c_str(), ',',  wPair, 5) ;
      for (int i=0; i<wp; i++)
      {
        //DebugTf("[%d] -> pair[%s]\r\n", i, wPair[i].c_str());
        int8_t wc = splitString(wPair[i].c_str(), ':',  wOut, 5) ;
        //DebugTf("==> [%s] -> field[%s]->val[%s]\r\n", wPair[i].c_str(), wOut[0].c_str(), wOut[1].c_str());
        if (wOut[0].equalsIgnoreCase("name"))  strCopy(field, sizeof(field), wOut[1].c_str());
        if (wOut[0].equalsIgnoreCase("value")) strCopy(newValue, sizeof(newValue), wOut[1].c_str());
      }
      DebugTf("--> field[%s] => newValue[%s]\r\n", field, newValue);
      //--> updateSetting(field, newValue);
      response->print(jsonChar);

} // postSettings()


//====================================================
void sendApiNotFound(AsyncWebServerRequest *request, const char *URI)
{
  AsyncResponseStream *response = request->beginResponseStream("text/html"); 
  
  response->addHeader("Access-Control-Allow-Origin", "*");
//response->setContentLength(CONTENT_LENGTH_UNKNOWN);
  response->print("<!DOCTYPE HTML><html><head>");
  response->print("<style>body { background-color: lightgray; font-size: 15pt;}");
  response->print("</style></head><body>");
  response->print("<h1>ESP32_Framework</h1><b1>");
  response->print("<br>[<b>");
  response->print(URI);
  response->print("</b>] is not a valid ");
  
  response->print("<h3>Headers</h3>");
  response->print("<ul>");
  int headers = request->headers();
  for(int i=0;i<headers;i++){
    AsyncWebHeader* h = request->getHeader(i);
    response->printf("<li>%s: %s</li>", h->name().c_str(), h->value().c_str());
  }
  response->print("</ul>");

  response->print("<h3>Parameters</h3>");
  response->print("<ul>");
  int params = request->params();
  for(int i=0;i<params;i++){
    AsyncWebParameter* p = request->getParam(i);
    if(p->isFile()){
      response->printf("<li>FILE[%s]: %s, size: %u</li>", p->name().c_str(), p->value().c_str(), p->size());
    } else if(p->isPost()){
      response->printf("<li>POST[%s]: %s</li>", p->name().c_str(), p->value().c_str());
    } else {
      response->printf("<li>GET[%s]: %s</li>", p->name().c_str(), p->value().c_str());
    }
  }
  response->print("</ul>");

  response->print("</body></html>");
  //send the response last
  request->send(response);

} // sendApiNotFound()


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
