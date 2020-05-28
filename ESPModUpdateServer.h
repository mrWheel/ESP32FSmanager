/* 
***************************************************************************  
**  THIS HAS TO BE MOVED TO A LIBRARY **
***************************************************************************  
**  Program  : ESPModUpdateServer.h
**  Version  : v0.0.1
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
**  
**  This is a hack based on sample code from: 
**  https://github.com/lbernstone/asyncUpdate/blob/master/AsyncUpdate.ino 
**  
**  and modified by Willem Aandewiel.
**
***************************************************************************      
*/

#ifndef ESP_MOD_UPDATE_SERVER_H
#define ESP_MOD_UPDATE_SERVER_H

#ifndef Debug
  //#warning Debug() was not defined!
  #define Debug(...)    ({ Debug(__VA_ARGS__); })  
  #define Debugln(...)  ({ Debugln(__VA_ARGS__); })  
  #define Debugf(...)   ({ Debugf(__VA_ARGS__); })  
//#else
//  #warning Seems Debug() is already defined!
#endif

#include <ESPAsyncWebServer.h>
#ifdef ESP8266
  #include <Updater.h>
  #include <ESP8266mDNS.h>
#else
  #include <Update.h>
  #include <ESPmDNS.h>
#endif

class ESPModUpdateServer
{

public:
ESPModUpdateServer() 
{
  static const char _defaultIndex[] = R"(
            <form method='POST' action='/doUpdate' enctype='multipart/form-data'>
              <input type='file' name='update'>
              <input type='submit' value='Update'>
            </form>)";
  static const char _defaultSuccess[] = R"(
            <html charset="UTF-8">
            <body>
              <h2>Update successful!</h2>
              <br/>
              <br/>Wait <span style='font-size: 1.3em;' id="waitSeconds">10</span> seconds ..
            </body>
            <script>
              var seconds = document.getElementById("waitSeconds").textContent;
              var countdown = setInterval(function() 
              {
                seconds--;
                document.getElementById('waitSeconds').textContent = seconds;
                if (seconds <= 0) {
                  clearInterval(countdown);
                  window.location.assign("/")
                }
              }, 1000);
            </script>
            </html>)";

  setIndexPage(_defaultIndex);  
  setSuccessPage(_defaultSuccess);

} // ESPModUpdateServer();

/**
void begin()
{
  static const char _defaultIndex[] = R"(
            <form method='POST' action='/doUpdate' enctype='multipart/form-data'>
              <input type='file' name='update'>
              <input type='submit' value='Update'>
            </form>)";
  static const char _defaultSuccess[] = R"(
            <html charset="UTF-8">
            <body>
              <h2>Update successful!</h2>
              <br/>
              <br/>Wait <span style='font-size: 1.3em;' id="waitSeconds">10</span> seconds ..
            </body>
            <script>
              var seconds = document.getElementById("waitSeconds").textContent;
              var countdown = setInterval(function() 
              {
                seconds--;
                document.getElementById('waitSeconds').textContent = seconds;
                if (seconds <= 0) {
                  clearInterval(countdown);
                  window.location.assign("/")
                }
              }, 1000);
            </script>
            </html>)";

  setIndexPage(_defaultIndex);  
  setSuccessPage(_defaultSuccess);

} // begin();
**/

  //====================================================================
  void setIndexPage(const char *indexPage)
  {
    _serverIndex = indexPage;
  }

  //====================================================================
  void setSuccessPage(const char *successPage)
  {
    _serverSuccess = successPage;
  }  
  
  //====================================================================
  void handleUpdate(AsyncWebServerRequest *request) 
  {
    request->send(200, "text/html", _serverIndex);
  }


  //====================================================================
  void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) 
  {
    if (!index)
    {
      Debugln("Update");
      _content_len = request->contentLength();
      // if filename includes spiffs, update the spiffs partition
      int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
  #ifdef ESP8266
      Update.runAsync(true);
      if (!Update.begin(content_len, cmd)) 
      {
  #else
      if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) 
      {
  #endif
        Update.printError(Serial);
        Update.printError(TelnetStream);
      }
    }

    if (Update.write(data, len) != len) 
    {
      Update.printError(Serial);
      Update.printError(TelnetStream);
    } 
    else 
    {
  #ifdef ESP8266
      Debugf("Progress: %d%%\r\n", (Update.progress()*100)/Update.size());
  #else
      Debug(".");
  #endif
    }

    if (final) 
    {
      Debugln(" done!");
      AsyncWebServerResponse *response = request->beginResponse(302, "text/html", _serverSuccess);
      request->send(response);
      if (!Update.end(true))
      {
        Update.printError(Serial);
      } 
      else 
      {
        Debug("Update complete [");
        Debug(_content_len);
        Debugln(" bytes]\r\nReboot in 1 second\r");
        DebugFlush();
        delay(1000);
        ESP.restart();
      }
    }
  } // handleDoUpdate()


//====================================================================
  void printProgress(size_t prg, size_t sz) 
  {
    Debugf("Progress: %d%%\n", (prg*100)/sz);
  
  } // printProgress()


  
private:
  AsyncWebServer *_server;
  size_t _content_len;
  const char *_path;
  const char *_serverIndex;
  const char *_serverSuccess;

}; // end Class ESPModUpdateServer()

#endif


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
***************************************************************************/
