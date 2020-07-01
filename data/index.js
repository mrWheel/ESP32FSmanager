/*
***************************************************************************  
**  Program  : index.js   part of Framework
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
  console.log("now continue with the bootstrapMain");
  const APIGW='http://'+window.location.host+'/api/';

  "use strict";

  var needReload  = true;
  var timeTimer;
 
  window.onload=bootsTrapMain;
  window.onfocus = function() {
    if (needReload) {
      window.location.reload(true);
    }
  };
    
  //============================================================================  
  function bootsTrapMain() 
  {
    console.log("bootsTrapMain() ");
    
    refreshDevTime();
    refreshDevInfo();
    
    document.getElementById('Knop1').addEventListener('click',function() 
                                                {handleKnop(1);});
    document.getElementById('Knop2').addEventListener('click',function() 
                                                {handleKnop(2);});
    document.getElementById('Settings').addEventListener('click',function() 
                                                {settingsPage();});
    document.getElementById('M_FSexplorer').addEventListener('click',function() 
                                                { console.log("newTab: goFSexplorer");
                                                  location.href = "/FSexplorer";
                                                });
    document.getElementById('S_FSexplorer').addEventListener('click',function() 
                                                { console.log("newTab: goFSexplorer");
                                                  location.href = "/FSexplorer";
                                                });
    document.getElementById('back').addEventListener('click',function() 
                                                { console.log("newTab: goBack");
                                                  location.href = "/";
                                                });
    needReload = false;

    clearInterval(timeTimer);  
    timeTimer = setInterval(refreshDevTime, 30 * 1000); // repeat every 30s

    document.getElementById("displayMainPage").style.display       = "block";
    document.getElementById("displaySettingsPage").style.display   = "none";
    //refreshDevInfo();

    document.getElementById("M_FSexplorer").src="/FSexplorer.png";
    document.getElementById("Settings").src="/settings.png";
    document.getElementById("S_FSexplorer").src="/FSexplorer.png";
    document.getElementById("Settings").src="/settings.png";

  } // bootsTrapMain()

  function settingsPage()
  {
    document.getElementById("displayMainPage").style.display       = "none";

    var settingsPage = document.getElementById("settingsPage");
    //refreshSettings();
    document.getElementById("displaySettingsPage").style.display   = "block";
    
  } // settingsPage()

  
  //============================================================================  
  function refreshDevTime()
  {
    //console.log("Refresh api/v0/devtime ..");
    fetch(APIGW+"v0/get/devtime")
      .then(response => response.json())
      .then(json => {
        //format:
        //{"devtime":{"datetime":"<the time>"}};
        devtime = json['devtime'];
        for( let key in devtime )
        {
          console.log("devtime["+key+"] => ["+devtime[key]+"]");
          if (key == "datetime")
          {
            console.log("Found key==datetime => value["+devtime[key]+"]");
            document.getElementById('DateTime').innerHTML = devtime[key];
          }
        }
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });     
      
  } // refreshDevTime()
    
  
  //============================================================================  
  function refreshDevInfo()
  {
    document.getElementById('devName').innerHTML = "";
    fetch(APIGW+"v0/get/devinfo")
      .then(response => response.json())
      .then(json => {
        //console.log("parsed .., data is ["+ JSON.stringify(json)+"]");
        data = json.devinfo;
        for( let i in data )
        {
            if (data[i].name == "fwversion")
            {
              document.getElementById('devVersion').innerHTML = json.devinfo[i].value;

            } else if (data[i].name == 'hostname')
            {
              document.getElementById('devName').innerHTML += data[i].value+" ";
              
            } else if (data[i].name == 'ipaddress')
            {
              document.getElementById('devName').innerHTML += " ("+data[i].value+") ";
            }
        }
      })
      .catch(function(error) {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });     
  } // refreshDevInfo()
  
  //============================================================================  
  function handleKnop(knopNr) 
  {
    console.log("handleKnop("+knopNr+") ...");
    
  } // handleKnop()
  
/*
***************************************************************************
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
***************************************************************************
*/
