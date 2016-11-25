#include "webserver.h"




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
        if (serverweb.arg("SSID") == "admin" &&  serverweb.arg("PASSWORD") == "admin" )
        {
            String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=1\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
            serverweb.sendContent(header);
            Serial.println("Log in Successful");
            return;
        }
        msg = serverweb.arg("MAXCOUNTER");
        msg+="\n"+serverweb.arg("MAXCOUNTER_ALT");
        msg+="\n"+serverweb.arg("GUIDE");
        msg+="\n"+serverweb.arg("CENTER");
        msg+="\n"+serverweb.arg("FIND");
        msg+="\n"+serverweb.arg("SLEW");
        msg+="\n"+serverweb.arg("LONGITUDE");
        msg+="\n"+serverweb.arg("LATITUDE");
        msg+="\n"+serverweb.arg("TIMEZONE")+"\n";
        String temp=serverweb.arg("SLEW");
        telescope->rate[3]=temp.toFloat();
        Serial.println("Log in Failed");

        File f = SPIFFS.open("/mount.config", "w");
        if (!f)
        {
           msg=("file open failed");
        } else
        f.println(msg);
        f.close ();
    }
    String content = "<html><body><form action='/config' method='POST'><h2>ESP-PGT++ Config</h2><br>";
    content+="<fieldset> <legend>Login  information:</legend>";
    content+= "SSID:<br><input type='text' name='SSID' style='height:20px; width:80px' placeholder='SSID'><br>";
    content+= "Password:<br><input type='password' name='PASSWORD' style='height:20px; width:80px' placeholder='password'><br></fieldset>";
    content+="<fieldset> <legend>Mount parameters:</legend>";
    content+= "RA steps/rev:<br><input type='number'   name='MAXCOUNTER' style='height:20px; width:80px'  placeholder='Gear' value ='" + String(telescope->azmotor->maxcounter) + "'><br>";
    content+= "DEC steps/rev:<br><input type='number' name='MAXCOUNTER_ALT' style='height:20px; width:80px' placeholder='Gear' value ='" + String(telescope->altmotor->maxcounter) + "'><br>";
    content+= "Guide rate:<br><input type='number' step='any' name='GUIDE'style='height:20px; width:50px'  placeholder='GUIDE' value ='" + String(telescope->rate[0]) + "'><br>";
    content+= "Center rate:<br><input type='number' name='CENTER' style='height:20px; width:50px'placeholder='CENTER' value ='" + String(telescope->rate[1]) + "'><br>";
    content+= "Find rate:<br><input type='number' name='FIND' style='height:20px; width:50px' placeholder='FIND' value ='" + String(telescope->rate[2]) + "'><br>";
    content+= "Slew rate:<br><input type='number' name='SLEW' style='height:20px; width:50px'placeholder='SLEW' value ='" + String(telescope->rate[3]) + "'><br></fieldset>";
    content+="<fieldset> <legend>Geodata</legend>";
    content+= "Longitude:<br><input type='number' step='any' name='LONGITUDE' style='height:20px; width:80px'  placeholder='Long' value ='" + String(telescope->longitude) + "'><br>";
    content+= "Latitude:<br><input type='number'step='any' name='LATITUDE' style='height:20px; width:80px' placeholder='Lat' value ='" + String(telescope->lat) + "'><br>";
    content+= "GMT offset:<br><input type='number'step='any' name='TIMEZONE' style='height:20px; width:30px'  placeholder='TZ' value ='" + String(telescope->time_zone) + "'><br>";
    content+= "<input type='submit' name='SUBMIT' value='Submit'></fieldset></form>" + msg + "<br>";
    content+= " </body></html>";
    serverweb.send(200, "text/html", content);
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
    serverweb.onNotFound([]()
    {
        if (!handleFileRead(serverweb.uri()))
            serverweb.send(404, "text/plain", "FileNotFound");
    });
    serverweb.begin();
}
