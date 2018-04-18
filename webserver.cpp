#include <ESP8266WebServer.h>
#include "time.h"
#include "webserver.h"

long getDecimal(float val)
{
 int intPart = int(val);
 long decPart = 10000 *(val-intPart); //I am multiplying by 1000 assuming that the foat values will have a maximum of 3 decimal places
                                   //Change to match the number of decimal places you need
 if(decPart>0)return(decPart);           //return the decimal part of float number if it is available 
 else if(decPart<0)return((-1)*decPart); //if negative, multiply by -1
 else if(decPart=0)return(00);           //return 0 if decimal part of float number is not available
}
 


String getContentType(String filename)
{
    if (serverweb.hasArg("download")) return "application/octet-stream";
    else if (filename.endsWith(".htm")) return "text/html";
    else if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    else if (filename.endsWith(".xml")) return "text/xml";
    else if (filename.endsWith(".pdf")) return "application/x-pdf";
    else if (filename.endsWith(".zip")) return "application/x-zip";
    else if (filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}
void handleConfig()
{
    String msg;

    if (serverweb.hasArg("SSID") && serverweb.hasArg("PASSWORD"))
    {
        String ssid=serverweb.arg("SSID")+"\n"+serverweb.arg("PASSWORD")+"\n";
         File f = SPIFFS.open("/wifi.config", "w");
        if (!f)
        {
            ssid=("file open failed");
        }
        else
            f.println(ssid);
        f.close ();



        msg = serverweb.arg("MAXCOUNTER");
        msg+="\n"+serverweb.arg("MAXCOUNTER_ALT");
        msg+="\n"+serverweb.arg("GUIDE");
        msg+="\n"+serverweb.arg("CENTER");
        msg+="\n"+serverweb.arg("FIND");
        msg+="\n"+serverweb.arg("SLEW");
        msg+="\n"+serverweb.arg("PRESCALER");
        msg+="\n"+serverweb.arg("LONGITUDE");
        msg+="\n"+serverweb.arg("LATITUDE");
        msg+="\n"+serverweb.arg("TIMEZONE")+"\n";
        String temp=serverweb.arg("SLEW");
        telescope->rate[3]=temp.toFloat();
        Serial.println("Log in Failed");

        f = SPIFFS.open("/mount.config", "w");
        if (!f)
        {
            msg=("file open failed");
        }
        else
            {f.println(msg); readconfig(telescope);}
        f.close ();
    }
  //  String Jsc="<script>function getLocation() {if (navigator.geolocation) {navigator.geolocation.getCurrentPosition(showPosition);";
  //  Jsc+="} else {x.innerHTML = \"Geolocation not\";}}";
  //  Jsc+=" function showPosition(position) {document.getElementById('lon').value =  position.coords.longitude;document.getElementById('lat').value =  position.coords.latitude;}</script>";

    String content = "<html><body  bgcolor=\"#000000\" text=\"#FFFFFF\"><form action='/config' method='POST'><h2>ESP-PGT++ Config</h2>";
   // content+=Jsc;
    content+="<button onclick=\"location.href='/park'\"  type=\"button\">Park telescope</button>";
    content+="<button onclick=\"location.href='/restart'\"  type=\"button\">Restart device</button><br>";
    content+="<fieldset style=\"width:30%\"> <legend>Login  information:</legend>";
    content+= "SSID:<input type='text' name='SSID' style='height:20px; width:80px' value='"+ssi +"'>   ";
    content+= "Password:<input type='password' name='PASSWORD' style='height:20px; width:80px'  value='"+pwd+"'><br></fieldset>";
    content+="<fieldset style=\"width:30%\"> <legend>Mount parameters:</legend>";
    content+= "RA   steps/rev: <input type='number' name='MAXCOUNTER' style='height:20px; width:80px' value ='" + String(telescope->azmotor->maxcounter) + "'><br>";
    content+= "DEC steps/rev: <input type='number' name='MAXCOUNTER_ALT' style='height:20px; width:80px' value ='" + String(telescope->altmotor->maxcounter) + "'><br>";
    content+= "Guide rate:<input type='number' step='any' name='GUIDE'style='height:20px; width:50px' value ='" + String(telescope->rate[0]) + "'><br>";
    content+= "Center rate:<input type='number' name='CENTER' style='height:20px; width:50px' value ='" + String(telescope->rate[1]) + "'><br>";
    content+= "Find rate:<input type='number' name='FIND' style='height:20px; width:50px' value ='" + String(telescope->rate[2]) + "'><br>";
    content+= "Slew rate:<input type='number' name='SLEW' style='height:20px; width:50px' value ='" + String(telescope->rate[3]) + "'><br>";
    content+= "Prescaler:<input type='number' name='PRESCALER' style='height:20px; width:50px' value ='" + String(telescope->prescaler) + "'><br></fieldset>";
    content+="<fieldset style=\"width:30%\"> <legend>Geodata</legend>";
    content+= "Longitude:<input type='number' step='any' id=\"lon\" name='LONGITUDE' style='height:20px; width:80px' value ='"+ 
    String(int(telescope->longitude))+"." +String(getDecimal(telescope->longitude))+"'>   ";
    content+= "Latitude:<input type='number'step='any' id=\"lat\" name='LATITUDE' style='height:20px; width:80px' value ='" + // String(telescope->lat) +"'><br>";
    String(int(telescope->lat))+"." +String(getDecimal(telescope->lat))+"'>   ";
  //  content+= "<button onclick=\"getLocation()\">Try It</button><br>";
    content+= "GMT offset:<input type='number'step='any' name='TIMEZONE' style='height:20px; width:30px' value ='" + String(telescope->time_zone) + "'><br>";
    content+= "<input type='submit' name='SUBMIT' value='Submit'></fieldset></form>" + msg + "<br>";
    content+= " </body></html>";
    serverweb.send(200, "text/html", content);
}
void handlePark(void)
{ time_t now;
  now = time(nullptr);
  mount_park(telescope);
  String content =  "<html><body  bgcolor=\"#000000\" text=\"#FFFFFF\"><h2>ESP-PGT++ PARKED</h2><br>";
content+="Mount parked  ,position saved on EEPROM.<br>";
content+="AZ Counter:"+String(telescope->azmotor->counter)+"<br>";
content+="Alt Counter:"+String(telescope->altmotor->counter)+"<br>";
content+= "TIme :"+String(ctime(&now))+"<br>";
content+="<button onclick=\"location.href='/'\"  type=\"button\">Back</button><br>";
content+= "</body></html>";
 
  serverweb.send(200, "text/html", content);
}
void handleRestart(void)
{ mount_park(telescope);
  String content =   "<html><body  bgcolor=\"#000000\" text=\"#FFFFFF\"><h2>ESP-PGT++ restarted</h2><br>";
content+="Mount parked  ,position saved on EEPROM.<br>";
content+="AZ Counter:"+String(telescope->azmotor->counter)+"<br>";
content+="Alt Counter:"+String(telescope->altmotor->counter)+"<br>";
content+= "</body></html>";
serverweb.send(200, "text/html", content);
 ESP.restart(); 
}
bool handleFileRead(String path)
{

    if (path.endsWith("/")) path += "index.htm";
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
    {
        if (SPIFFS.exists(pathWithGz))
            path += ".gz";
        File file = SPIFFS.open(path, "r");
        size_t sent = serverweb.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}
void initwebserver(void)
{
    serverweb.on("/config", handleConfig);
    serverweb.on("/", handleConfig);
     serverweb.on("/park", handlePark);
     serverweb.on("/restart", handleRestart);
   // serverweb.on("/formatfilesystem",handleFormat)
    serverweb.onNotFound([]()
    {
        if (!handleFileRead(serverweb.uri()))
            serverweb.send(404, "text/plain", "FileNotFound");
    });
    serverweb.begin();
}
