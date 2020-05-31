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

//=======================================================================
void sendStartJsonObj(AsyncResponseStream *response, const char *objName)
{
  char sBuff[50];
  objSprtr[0]    = '\0';
  size_t buffLen, index;
  size_t maxLen = 1024;

  if (Verbose) DebugTln("startJson");
  if (Verbose) DebugFlush();


  snprintf(sBuff, 49, "{\"%s\":[\r\n", objName);
  response->print(sBuff);
  
} // sendStartJsonObj()


//=======================================================================
void sendEndJsonObj(AsyncResponseStream *response)
{
  char sBuff[50] = "";

  if (Verbose) DebugTln("endJson");
  if (Verbose) DebugFlush();

  snprintf(sBuff, sizeof(sBuff), "\r\n]}\r\n");
  
  response->print(sBuff);
  
} // sendEndJsonObj()


//=======================================================================
void sendNestedJsonObj(AsyncResponseStream *response, const char *cName, const char *cValue)
{
  char jsonBuff[JSON_BUFF_MAX] = "";

  if (Verbose) DebugTln("nestedJson");
  if (Verbose) DebugFlush();

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": \"%s\"}"
                                      , objSprtr, cName, cValue);

  response->print(jsonBuff);
  
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, *char)


//=======================================================================
void sendNestedJsonObj(AsyncResponseStream *response, const char *cName, String sValue)
{
  char jsonBuff[JSON_BUFF_MAX] = "";

  if (Verbose) DebugTln("nestedJson");
  if (Verbose) DebugFlush();
    
  if (sValue.length() > (JSON_BUFF_MAX - 65) )
  {
    DebugTf("[2] sValue.length() [%d]\r\n", sValue.length());
  }
  
    snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": \"%s\"}"
                                     , objSprtr, cName, sValue.c_str());
  response->print(jsonBuff);
  
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, String)


//=======================================================================
void sendNestedJsonObj(AsyncResponseStream *response, const char *cName, int32_t iValue)
{
  char jsonBuff[200] = "";

  if (Verbose) DebugTln("nestedJson");
  if (Verbose) DebugFlush();
    
  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %d}"
                                      , objSprtr, cName, iValue);

  response->print(jsonBuff);
  
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, int)

//=======================================================================
void sendNestedJsonObj(AsyncResponseStream *response, const char *cName, uint32_t uValue)
{
  char jsonBuff[200] = "";

  if (Verbose) DebugTln("nestedJson");
  if (Verbose) DebugFlush();
  
  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %u }"
                                      , objSprtr, cName, uValue);
  response->print(jsonBuff);
  
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, uint)


//=======================================================================
void sendNestedJsonObj(AsyncResponseStream *response, const char *cName, float fValue)
{
  char jsonBuff[200] = "";

  if (Verbose) DebugTln("nestedJson");
  if (Verbose) DebugFlush();
  
  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.3f }"
                                      , objSprtr, cName, fValue);
  response->print(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, float)


//=======================================================================
// ************ function to build Json Settings string ******************
//=======================================================================
void sendJsonSettingObj(AsyncResponseStream *response, const char *cName, float fValue, const char *fType, int minValue, int maxValue)
{
  char jsonBuff[200] = "";
  //AsyncResponseStream *response;

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.3f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, fValue, fType, minValue, maxValue);
  response->print(jsonBuff);

  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, float, *char, int, int)


//=======================================================================
void sendJsonSettingObj(AsyncResponseStream *response, const char *cName, float fValue, const char *fType, int minValue, int maxValue, int decPlaces)
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
  response->print(jsonBuff);

  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, float, *char, int, int, int)


//=======================================================================
void sendJsonSettingObj(AsyncResponseStream *response, const char *cName, int iValue, const char *iType, int minValue, int maxValue)
{
  char jsonBuff[200] = "";
  //AsyncResponseStream *response;

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %d, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, iValue, iType, minValue, maxValue);

  response->print(jsonBuff);

  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, int, *char, int, int)


//=======================================================================
void sendJsonSettingObj(AsyncResponseStream *response, const char *cName, uint32_t uiValue, const char *iType, int minValue, int maxValue)
{
  char jsonBuff[200] = "";
  //AsyncResponseStream *response;

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %u, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
                                      , objSprtr, cName, uiValue, iType, minValue, maxValue);

  response->print(jsonBuff);

  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, uint, *char, int, int)


//=======================================================================
void sendJsonSettingObj(AsyncResponseStream *response, const char *cName, const char *cValue, const char *sType, int maxLen)
{
  char jsonBuff[200] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\":\"%s\", \"type\": \"%s\", \"maxlen\": %d}"
                                      , objSprtr, cName, cValue, sType, maxLen);
  response->print(jsonBuff);

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
