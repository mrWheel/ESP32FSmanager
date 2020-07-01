/* 
***************************************************************************  
**  Program  : jsonStuff, part of ESP32_Framework
**  Version  : v2.0.1
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
static char objSprtr[10] = "";


//===========================================================================================
void parseJsonKey(const char *sIn, const char *key, char *val, int valLen)
{
  bool stringType;
  // json key-value pair looks like:
  //      "samenv": "Zwaar bewolkt",
  // or   "samenv": "Zwaar bewolkt"}
  // json string:
  // {"field1": "val1","field2": "val2","field3": int3,"field4": "val4"}
  
  //DebugTf("[%s]\r\n", sIn);
  //--- field2
  int keyStart   = strIndex(sIn, key);
  if (keyStart == -1)
  {
    val[0] = 0;
    return;
  }
  int semStart   = strIndex(sIn, ":", (keyStart + strlen(key)));
  // {"field1": "val1","field2":  "val2","field3": int3,"field4": "val4"}
  //      keyStart(19)--^       ^--(27)semStart

  //--- now look for ["] or [0-9]
  stringType = false;
  int valStart;
  //-DebugT("valStart[");
  for (valStart=(semStart+1); sIn[valStart] != 0; valStart++)
  {
    if (sIn[valStart] == '"')
    {
      stringType = true;
      valStart++;
      break;
    }
    else if ((sIn[valStart] >= '0' && sIn[valStart] <= '9') || sIn[valStart] == '.') 
    {
      break;
    }
    //-Debug(sIn[valStart]);
  }
  //-Debugf("] (%s)\r\n", stringType ? "String" : "Number");
  
  // {"field1": "val1","field2":  "val2","field3": int3,"field4": "val4"}
  //      keyStart(19)--^         ^--(29)valStart

  //-DebugTf("Looking for [%s], found at [%d] ':'[%d] => valStart[%d]\r\n", key, keyStart, semStart, valStart);
  int valEnd;
  //-DebugTf("valEnd[%c", sIn[valStart]);
  for (valEnd = (valStart+1); valEnd < strlen(sIn); valEnd++)
  {
    if (stringType)
    {
      if (sIn[valEnd] == '"')
      {
        valEnd--;
        break;
      }
    }
    else  // number
    {
      if (sIn[valEnd] == ' ' || sIn[valEnd] == ',' || sIn[valEnd] == '}')
      {
        valEnd--;
        break;
      }
    }
    //-Debug(sIn[valEnd]);
  }
  //-Debugln("]");

  // {"field1": "val1","field2":  "val2","field3": int3,"field4": "val4"}
  //                    ^          ^  ^--(33)valEnd
  //      keyStart(19)--^          ^--(30)valStart

  //-DebugTf("valStart[%d], valEnd[%d]\r\n", valStart, valEnd);
  //-Debugf("%s\r\n", sIn);
  //-Debug("");
  //-for(int i=0; i<valStart; i++) Debugf("%d", i%10);
  //-Debug("^");
  //-for(int i=(valStart+1); i<valEnd; i++) Debug(" ");
  //-Debugln("^");
  //-- now copy from 30 upto 35 -> [val2",]
  strCopy(val, valLen, sIn, valStart, valEnd);
  DebugTf("[%s] => [%s]\r\n", key, val);
  
} // parseJsonKey()


//------------------------------------------------------
//  { "objName": [{ <sendStartJsonArrayObj>             --\
//        "key1a": "val1a"  <sendJsonObj>   --\            |
//       ,"key2a": "val2a"  <sendJsonObj>      | object    |
//       ,"key3a": "val3a"  <sendJsonObj>      |           |
//      },{          <sendNextJsonArrayObj> --/            | array
//        "key1a": "val1b"  <sendJsonObj>   --\            |
//       ,"key2a": "val2b"  <sendJsonObj>      | object    |
//       ,"key3a": "val3b"  <sendJsonObj    --/            |
//    }]}            <sendEndJsonArrayObj>              --/
//
//
//  { "objName": { <sendStartJsonObj> --\
//         "key1": "val1"   <sendJsonObj> --\           
//        ,"key2": "val2"   <sendJsonObj>    | object    
//        ,"key3": "val3"   <sendJsonObj> --/          
//  }}              <sendEndJsonObj>
//
//  {"key1a": "val1a"}    << object
//------------------------------------------------------


//=======================================================================
void sendStartJsonObj(const char *objName)
{
  char sBuff[50];
  objSprtr[0]    = '\0';
  size_t buffLen, index;
  size_t maxLen = 1024;

  if (Verbose) DebugTln("startJson");
  if (Verbose) DebugFlush();


  snprintf(sBuff, 49, "{\"%s\":\r\n  {\r\n", objName);
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer.send(200, "application/json", sBuff);
  
} // sendStartJsonObj()


//=======================================================================
void sendStartJsonArrayObj(const char *objName)
{
  char sBuff[50];
  objSprtr[0]    = '\0';
  size_t buffLen, index;
  size_t maxLen = 1024;

  if (Verbose) DebugTln("startJson");
  if (Verbose) DebugFlush();


  snprintf(sBuff, 49, "{\"%s\":[{\r\n", objName);
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer.send(200, "application/json", sBuff);
  
} // sendStartJsonArrayObj()


//=======================================================================
void sendNextJsonArrayObj()
{
  char sBuff[50] = "";

  if (Verbose) DebugTln("nextJson");
  if (Verbose) DebugFlush();

  snprintf(sBuff, sizeof(sBuff), "\r\n  },{\r\n");
  objSprtr[0]    = '\0';
  
  httpServer.sendContent(sBuff);
  
} // sendNextArrayJsonObj()


//=======================================================================
void sendEndJsonObj()
{
  char sBuff[50] = "";

  if (Verbose) DebugTln("endJson");
  if (Verbose) DebugFlush();

  snprintf(sBuff, sizeof(sBuff), "\r\n  }\r\n}\r\n");
  
  httpServer.sendContent(sBuff);
  
} // sendEndJsonObj()


//=======================================================================
void sendEndJsonArrayObj()
{
  char sBuff[50] = "";

  if (Verbose) DebugTln("endJson");
  if (Verbose) DebugFlush();

  snprintf(sBuff, sizeof(sBuff), "\r\n  }]\r\n}\r\n");
  
  httpServer.sendContent(sBuff);
  
} // sendEndJsonArrayObj()


//=======================================================================
void sendJsonObj(const char *cName, const char *cValue)
{
  char jsonBuff[JSON_BUFF_MAX] = "";

  if (Verbose) DebugTln("nestedJson");
  if (Verbose) DebugFlush();

  snprintf(jsonBuff, sizeof(jsonBuff), "%s   \"%s\": \"%s\""
                                      , objSprtr, cName, cValue);

  httpServer.sendContent(jsonBuff);
  
  sprintf(objSprtr, ",\r\n");

} // sendJsonObj(*char, *char)


//=======================================================================
void sendJsonObj(const char *cName, String sValue)
{
  char jsonBuff[JSON_BUFF_MAX] = "";

  if (Verbose) DebugTln("nestedJson");
  if (Verbose) DebugFlush();
    
  if (sValue.length() > (JSON_BUFF_MAX - 65) )
  {
    DebugTf("[2] sValue.length() [%d]\r\n", sValue.length());
  }
  
    snprintf(jsonBuff, sizeof(jsonBuff), "%s    \"%s\": \"%s\""
                                     , objSprtr, cName, sValue.c_str());
  httpServer.sendContent(jsonBuff);
  
  sprintf(objSprtr, ",\r\n");

} // sendJsonObj(*char, String)


//=======================================================================
void sendJsonObj(const char *cName, int32_t iValue)
{
  char jsonBuff[200] = "";

  if (Verbose) DebugTln("nestedJson");
  if (Verbose) DebugFlush();
    
  snprintf(jsonBuff, sizeof(jsonBuff), "%s    \"%s\": %d"
                                      , objSprtr, cName, iValue);

  httpServer.sendContent(jsonBuff);
  
  sprintf(objSprtr, ",\r\n");

} // sendJsonObj(*char, int)

//=======================================================================
void sendJsonObj(const char *cName, uint32_t uValue)
{
  char jsonBuff[200] = "";

  if (Verbose) DebugTln("nestedJson");
  if (Verbose) DebugFlush();
  
  snprintf(jsonBuff, sizeof(jsonBuff), "%s    \"%s\": %u"
                                      , objSprtr, cName, uValue);
  httpServer.sendContent(jsonBuff);
  
  sprintf(objSprtr, ",\r\n");

} // sendJsonObj(*char, uint)


//=======================================================================
void sendJsonObj(const char *cName, float fValue)
{
  char jsonBuff[200] = "";

  if (Verbose) DebugTln("nestedJson");
  if (Verbose) DebugFlush();
  
  snprintf(jsonBuff, sizeof(jsonBuff), "%s    \"%s\": %.3f"
                                      , objSprtr, cName, fValue);
  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendJsonObj(*char, float)


//=======================================================================
// ************ function to build Json Settings string ******************
//=======================================================================
void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue)
{
  char jsonBuff[200] = "";
  //AsyncResponseStream *response;

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.3f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);
  httpServer.sendContent(jsonBuff);

  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, float, *char, int, int)


//=======================================================================
void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue, int decPlaces)
{
  char jsonBuff[200] = "";

  switch(decPlaces) {
    case 0:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.0f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);
      break;
    case 2:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.2f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);
      break;
    case 5:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.5f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);
      break;
    default:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);

  }
  httpServer.sendContent(jsonBuff);

  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, float, *char, int, int, int)


//=======================================================================
void sendJsonSettingObj(const char *cName, int iValue, const char *iType, int minValue, int maxValue)
{
  char jsonBuff[200] = "";
  //AsyncResponseStream *response;

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"%s\": %d, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, iValue, iType, minValue, maxValue);

  httpServer.sendContent(jsonBuff);

  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, int, *char, int, int)


//=======================================================================
void sendJsonSettingObj(const char *cName, uint32_t uiValue, const char *iType, int minValue, int maxValue)
{
  char jsonBuff[200] = "";
  //AsyncResponseStream *response;

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %u, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, uiValue, iType, minValue, maxValue);

  httpServer.sendContent(jsonBuff);

  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, uint, *char, int, int)


//=======================================================================
void sendJsonSettingObj(const char *cName, const char *cValue, const char *sType, int maxLen)
{
  char jsonBuff[200] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\":\"%s\", \"type\": \"%s\", \"maxlen\": %d}"
                                      , objSprtr, cName, cValue, sType, maxLen);
  httpServer.sendContent(jsonBuff);

  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, *char, *char, int, int)




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
