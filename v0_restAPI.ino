/* 
***************************************************************************  
**  Program  : v0_restAPI, part of ESP32_Framework
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/


//=======================================================================
void v0_Get_restAPI() 
{
  char URI[50]   = "";
  char jsonString[300] = "";
  
  strncpy( URI, httpServer.uri().c_str(), sizeof(URI) );
  DebugT("from[");
  Debug(httpServer.client().remoteIP());
  Debugf("] URI[%s] method[GET] \r\n", URI);

  if (httpServer.hasArg("plain") == false) 
  {
    //DebugTln("No \"plain\" arg ..");
  }
  else
  {
    String body = httpServer.arg("plain");
    DebugTln("Body:");
    DebugTln(body);
  }

  if (strIndex("/api/v0/get/devtime", httpServer.uri().c_str()) >= 0)
  {
    char devTime[20] = "";
    sprintf(devTime, "%02d-%02d-%04d %02d:%02d:%02d", day(), month(), year()
                                                    , hour(), minute(), second());
    sendStartJsonObj("devtime");
    sendJsonObj("datetime", devTime);
    sendEndJsonObj();
    return;
  }

  if (strIndex("/api/v0/get/get_1_function", httpServer.uri().c_str()) >= 0)
  {
    if (httpServer.hasArg("var1"))
    {
      sendNewData(httpServer.arg("var1").c_str());
      return;
    }
    else
    {
      DebugTln("Something is wrong (missing 'var1' param?)");
    }
  }

  if (strIndex("/api/v0/get/get_2_function", httpServer.uri().c_str()) >= 0)
  {
    if (httpServer.hasArg("var1") && httpServer.hasArg("var2"))
    {
      sendSomeData(httpServer.arg("var1").c_str(), httpServer.arg("var2").c_str());
      return;
    }
    else if (httpServer.hasArg("var1"))
    {
      sendSomeData(httpServer.arg("var1").c_str());
      return;
    }
    else
    {
      DebugTln("Something is wrong (missing 'var1' or 'var2' param?)");
      httpServer.send(412, "text/plain", "[412] On, GET: "+String(URI)+" missing param(s)\r\n");
      return;
    }
  }
  
  httpServer.send(404, "text/plain", "[404] On, GET: "+String(URI)+"\r\n");
  
} // v0_Get_restAPI()


//=======================================================================
void v0_Post_restAPI() 
{
  char URI[50]          = "";
  char jsonString[300]  = "";
  
  strncpy( URI, httpServer.uri().c_str(), sizeof(URI) );
  DebugT("from[");
  Debug(httpServer.client().remoteIP());
  Debugf("] URI[%s] method[POST] \r\n", URI); 
  
  if (httpServer.hasArg("plain") == false) 
  {
    DebugTln("No \"plain\" arg ..");
  }
  else
  {
    snprintf(jsonString, sizeof(jsonString), httpServer.arg("plain").c_str());
    DebugT("jsonString: ");
    Debugln(jsonString);
  }
  
  if (strIndex("/api/v0/post/post_1_function", httpServer.uri().c_str()) >= 0)
  {
    DebugTln("executing post_1_function ..");
    httpServer.send(200);
  }
  else if(strIndex("/api/v0/post/post_2_function", httpServer.uri().c_str()) >= 0)
  {
    if (strlen(jsonString) > 0)
    {
      processPost(jsonString, strlen(jsonString)); 
    }
  }
  else
  {
    httpServer.send(404, "text/plain", "[404] On, POST: "+String(URI)+"\r\n");
  }
  
} // v0_Post_restAPI()


//=======================================================================
void sendNewData(const char *var1)
{
  DebugTf("var1 [%s]\r\n", var1);
// {
//   "newdata": [
//     {
//       "fld1": "val1",
//       "fld2": "val2"
//       "fld3": "val3",
//       "fld4": "val4",
//       "fld5": "val5",
//       "fld6": "val6"
//      }
// ]}
  sendStartJsonObj("newdata");
  sendJsonObj("fld1", "val1");
  sendJsonObj("fld2", "val2");
  sendJsonObj("fld3", "val3");
  sendJsonObj("fld4", "val4");
  sendJsonObj("fld5", "val5");
  sendJsonObj("fld6", "val6");
  sendEndJsonObj();
  //httpServer.send(response);
  DebugTf("newData send!\r\n");
  
} // sendNeworder()


//=======================================================================
void sendSomeData(const char *var1, const char *var2)
{
  DebugTf("var1 [%s] var2 [%s]\r\n", var1, var2);
  
// {
//   "somedata": {
//      "something": "someval1",
//      "status": "x”
//   }
// }

  httpServer.send(200); //--- OK
  DebugTf("get_2_function send!\r\n");
  
} // sendSomeData(var1, var2)


//=======================================================================
void sendSomeData(const char *var1)
{
  DebugTf("var1 [%s]\r\n", var1);
  
// {
//   "somedata": [{
//      "something": "someval1",
//      "status": "x”
//     },{
//      "something": "someval2",
//      "status": "x”
//     },{
//      "something": "someval3",
//      "status": "x”
//   }]
// }

  httpServer.send(200); //--- OK
  DebugTf("get_2_function send!\r\n");
  
} // sendSomeData(var1)


//=======================================================================
void processPost(char *jsonData, size_t jsonLen)
{
  char fld1[_FLD1]  = "";
  char fld2[_FLD2]  = "";
  char fld3[_FLD3]  = "";
  char fld4[_FLD4]  = "";

  parseJsonKey(jsonData, "fld1", fld1, _FLD1);
  parseJsonKey(jsonData, "fld2", fld2, _FLD2);
  parseJsonKey(jsonData, "fld3", fld3, _FLD3);
  parseJsonKey(jsonData, "fld4", fld4, _FLD4);
  
  DebugTf("fld1[%s], fld2[%s], fld3[%s], fld4[%s]\r\n", fld1  
                                                      , fld2
                                                      , fld3
                                                      , fld4);

  if (strlen(fld1) > 0 && strlen(fld2) > 0 && strlen(fld3) > 0 && strlen(fld4) > 0)
  {
    httpServer.send(200);
    DebugTln("[200] Done..");
  }
  else
  {
    httpServer.send(412);
    DebugTln("[412] Done..with error(s)!");
  }

} // processPost()


//=======================================================================
void postSettings(uint8_t *bodyData, size_t bodyLen)
{
  //------------------------------------------------------------ 
  // json string: {"name":"settingInterval","value":9}  
  // json string: {"name":"settingHostname","value":"abc"}  
  //------------------------------------------------------------ 
  // so, why not use ArduinoJSON library?
  // I say: try it yourself ;-) It won't be easy
      //AsyncResponseStream *response = httpServer.beginResponseStream("application/json");
      
      String wOut[5];
      String wPair[5];
      String jsonIn = "";
      char jsonChar[1024] = "";
      size_t i;
      DebugT("bodyData: ");
      for(i=0; (i<bodyLen && i < 1023); i++)
      {
        Debug((char)bodyData[i]);
        jsonChar[i] = (char)bodyData[i];
      }
      jsonChar[i] = '\0';
      Debugln();
      
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
      updateSetting(field, newValue);
      //response->print(jsonChar);
      //send the response last
      //httpServer.send(400, "plain/text", "response");

} // postSettings()


//====================================================
void sendApiNotFound(const char *URI)
{
  DebugTln("NotFound --> nog doen!!!");
  /**  
  response->addHeader("Access-Control-Allow-Origin", "*");
//response->setContentLength(CONTENT_LENGTH_UNKNOWN);
  response->print("<!DOCTYPE HTML><html><head>");
  response->print("<style>body { background-color: lightgray; font-size: 15pt;}");
  response->print("</style></head><body>");
  response->print("<h1>ESP32_Framework</h1><b1>");
  response->print("<br>[<b>");
  response->print(URI);
  response->print("</b>] is not a valid API call");
  
  response->print("<h3>Headers</h3>");
  response->print("<ul>");
  int headers = httpServer.headers();
  for(int i=0;i<headers;i++){
    AsyncWebHeader* h = httpServer.getHeader(i);
    response->printf("<li>%s: %s</li>", h.name().c_str(), h.value().c_str());
  }
  response->print("</ul>");
/**
  response->print("<h3>Parameters</h3>");
  response->print("<ul>");
  int params = httpServer.args();
  for(int i=0;i<params;i++){
    AsyncWebParameter* p = httpServer.arg(i);
    if(p->isFile()){
      response->printf("<li>FILE[%s]: %s, size: %u</li>", p.name().c_str(), p.value().c_str(), p->size());
    } else if(p->isPost()){
      response->printf("<li>POST[%s]: %s</li>", p.name().c_str(), p.value().c_str());
    } else {
      response->printf("<li>GET[%s]: %s</li>", p.name().c_str(), p.value().c_str());
    }
  }
  response->print("</ul>");
**/
/**
  response->print("</body></html>");
  //send the response last
  httpServer.send(response);
**/
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
