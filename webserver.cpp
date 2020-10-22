#include <ESP8266WebServer.h>
#include "time.h"
#include "webserver.h"

long getDecimal(float val)
{
  int intPart = int(val);
  long decPart = 10000 * (val - intPart); //I am multiplying by 1000 assuming that the foat values will have a maximum of 3 decimal places
  //Change to match the number of decimal places you need
  if (decPart > 0)return (decPart);       //return the decimal part of float number if it is available
  else if (decPart < 0)return ((-1) * decPart); //if negative, multiply by -1
  else if (decPart = 0)return (00);       //return 0 if decimal part of float number is not available
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
    String ssid = serverweb.arg("SSID") + "\n" + serverweb.arg("PASSWORD") + "\n";
    File f = SPIFFS.open("/wifi.config", "w");
    if (!f)
    {
      ssid = ("file open failed");
    }
    else
      f.println(ssid);
    f.close ();



    msg = serverweb.arg("MAXCOUNTER");
    msg += "\n" + serverweb.arg("MAXCOUNTER_ALT");
    msg += "\n" + serverweb.arg("GUIDE");
    msg += "\n" + serverweb.arg("CENTER");
    msg += "\n" + serverweb.arg("FIND");
    msg += "\n" + serverweb.arg("SLEW");
    msg += "\n" + serverweb.arg("GUIDEA");
    msg += "\n" + serverweb.arg("CENTERA");
    msg += "\n" + serverweb.arg("FINDA");
    msg += "\n" + serverweb.arg("SLEWA");
    msg += "\n" + serverweb.arg("PRESCALER");
    msg += "\n" + serverweb.arg("TRACK");
    msg += "\n" + serverweb.arg("LONGITUDE");
    msg += "\n" + serverweb.arg("LATITUDE");
    msg += "\n" + serverweb.arg("TIMEZONE") + "\n";
    String temp = serverweb.arg("SLEW");
    telescope->rate[3][0] = temp.toFloat();
    temp = serverweb.arg("SLEWA");
    telescope->rate[3][1] = temp.toFloat();
    Serial.println("Log in Failed");

    f = SPIFFS.open("/mount.config", "w");
    if (!f)
    {
      msg = ("file open failed");
    }
    else
    {
      f.println(msg);
      f.close ();
      readconfig(telescope);
    }
    f.close ();
  }
  //  String Jsc="<script>function getLocation() {if (navigator.geolocation) {navigator.geolocation.getCurrentPosition(showPosition);";
  //  Jsc+="} else {x.innerHTML = \"Geolocation not\";}}";
  //  Jsc+=" function showPosition(position) {document.getElementById('lon').value =  position.coords.longitude;document.getElementById('lat').value =  position.coords.latitude;}</script>";
  String content = "<html><style>" + String(BUTT) + String(TEXTT) + "</style>"+String(AUTO_SIZE)+"<body  bgcolor=\"#000000\" text=\"#FF6000\"><form action='/config' method='POST'><h2>ESP-PGT++ Config</h2>";
  // content+=Jsc;
  content += "<fieldset style=\"width:15%;border-radius:15px\"> <legend>Login  information:</legend>";
  content += "<table style='width:200px'>";
  content += "<tr><td>SSID</td><td><input type='text' name='SSID'  class=\"text_red\" value='" + ssi + "'></td></tr> ";
  content += "<tr><td>Password:</td><td><input type='password' name='PASSWORD'  class=\"text_red\"  value='" + pwd + "'></td>";
  content += "<td><button onclick=\"location.href='/network'\" class=\"button_red\"   type=\"button\">Network</button></td></tr></table></fieldset>";
  content += "<fieldset style=\"width:15%;border-radius:15px\"> <legend>Mount parameters:</legend>";
  content += "<table style='width:200px'><tr><th></th><th>RA</th><th>Dec</th></tr>";
  content += "<tr><td>Counter</td><td> <input type='number' name='MAXCOUNTER'  class=\"text_red\" value ='" + String(telescope->azmotor->maxcounter) + "'></td>";
  content += "<td> <input type='number' name='MAXCOUNTER_ALT'  class=\"text_red\" value ='" + String(telescope->altmotor->maxcounter) + "'></td></tr></table><br>";
  content += "<table style='width:200px'><tr><th>Rate X</th><th>RA/AZ</th><th>Dec/Alt</th></tr>";
  content += "<tr><td>Guide</td><td><input type='number' step='0.01' name='GUIDE' class=\"text_red\" value='" + String(telescope->rate[0][0]) + "'></td>";
  content += "<td><input type='number' step='0.01' name='GUIDEA' class=\"text_red\" value='" + String(telescope->rate[0][1]) + "'></td></tr>";

  content += "<tr><td>Center</td><td><input type='number' step='0.01' name='CENTER' class=\"text_red\" value='" + String(telescope->rate[1][0]) + "'></td>";
  content += "<td><input type='number' step='0.01' name='CENTERA' class=\"text_red\" value='" + String(telescope->rate[1][1]) + "'></td></tr>";

  content += "<tr><td>Find</td><td><input type='number' step='0.01' name='FIND' class=\"text_red\" value='" + String(telescope->rate[2][0]) + "'></td>";
  content += "<td><input type='number' step='0.01' name='FINDA'  class=\"text_red\" value ='" + String(telescope->rate[2][1]) + "'></td></tr>";

  content += "<tr><td>Slew</td><td><input type='number' step='0.01' name='SLEW' class=\"text_red\" value='" + String(telescope->rate[3][0]) + "'></td>";
  content += "<td><input type='number' step='0.01' name='SLEWA' class=\"text_red\" value='" + String(telescope->rate[3][1]) + "'></td></tr>";
  content += "<tr><td>Prescaler</td><td><input type='number' step='0.01' name='PRESCALER'  class=\"text_red\" value ='" + String(telescope->prescaler) + "' uSec</td></tr>";
  content += "<tr><td>Track</td><td><input type='number' name='TRACK'  class=\"text_red\" value ='" + String(telescope->track) + "' </td></tr></table></fieldset>";
  content += "<fieldset style=\"width:15%;border-radius:15px\"> <legend>Geodata</legend>";
  content += "<table style='width:200px'>";
  content += "<tr><td>Longitude:</td><td><input type='number' step='any' id=\"lon\" name='LONGITUDE'  class=\"text_red\" value ='" +
             String(int(telescope->longitude)) + "." + String(getDecimal(telescope->longitude)) + "'></td></tr>";
  content += "<tr><td>Latitude:</td><td><input type='number'step='any' id=\"lat\" name='LATITUDE'  class=\"text_red\" value ='" + // String(telescope->lat) +"'><br>";
             String(int(telescope->lat)) + "." + String(getDecimal(telescope->lat)) + "'></td></tr>";
  //  content+= "<button onclick=\"getLocation()\">Try It</button><br>";
  content += "<tr><td>GMT offset:</td><td><input type='number'step='1' name='TIMEZONE'  class=\"text_red\" value ='" + String(telescope->time_zone) + "'></td></tr></table>";
  content += "<input type='submit' name='SUBMIT' class=\"button_red\" value='Submit'></fieldset></form>" + msg + "<br>";
  content += "<button onclick=\"location.href='/park'\" class=\"button_red\"  type=\"button\">Park telescope</button>";
  content += "<button onclick=\"location.href='/restart'\" class=\"button_red\" type=\"button\">Restart device</button><br>";
  content += " </body></html>";
  serverweb.send(200, "text/html", content);
}
void handleSync(void)
{
  String msg;
  time_t rtc;
  if (serverweb.hasArg("GMT"))
  {
    msg = serverweb.arg("GMT");
    rtc = (msg.toInt());
    msg = serverweb.arg("OFFSET");
    timeval tv = { rtc, 0 };
    timezone tz = {msg.toInt()  , 0 };
    settimeofday(&tv, &tz);
    rtc = time(nullptr);
    //   telescope->is_tracking = one = FALSE;
    //  tak_init(telescope);
    //  telescope->is_tracking = TRUE;
  }
  String content =  "<html><style>" + String(BUTT) + String(TEXTT) + "</style>"+String(AUTO_SIZE)+"<body  bgcolor=\"#000000\" text=\"#FF6000\"><h2>ESP-PGT++ Sync </h2><br>";
  content += "<p id=\"fecha\">" + msg + " " + String(ctime(&rtc)) + "</p>";
  content += "<p id=\"fecha\">" + String(rtc) + "</p>";
  content += "<button onclick=\"location.href='/'\"  class=\"button_red\" type=\"button\">Back</button><br>";
  content += "</body></html>";
  serverweb.send(200, "text/html", content);

}
void handleTime(void)
{
  time_t now;
  now = time(nullptr);
  String content =  "<html><style>" + String(BUTT) + String(TEXTT) + "</style>"+String(AUTO_SIZE)+"<body  bgcolor=\"#000000\" text=\"#FF6000\"><h2>ESP-PGT++ Time </h2><br>";
  content += "<form id=\"frm1\" action=\"/sync\">";
  content += "<button onclick=\"myFunction()\"  class=\"button_red\">Synchronize now!</button>";
  content += "<input type=\"number\" name=\"GMT\"  class=\"text_red\" id=\"gmt\"  type=\"hidden\"><br><br>";
  content += "<input type=\"number\" name=\"OFFSET\"   class=\"text_red\" type=\"hidden\" id=\"offset\"><br><br>";
  content += "</form><p id=\"fecha\"></p>";
  content += "</form><p id=\"now\">" + String(now) + "</p>";
  content += "<button onclick=\"location.href='/'\"   class=\"button_red\" type=\"button\">Back</button><br>";
  content += "<script> var d = new Date();var n = ~~(Date.now()/1000);document.getElementById(\"gmt\").value = n; var n = d.toTimeString()+\" \"+d.toDateString();";
  content += "document.getElementById(\"fecha\").innerHTML = n; var o = d.getTimezoneOffset();";
  content += "document.getElementById(\"offset\").value =-o;";
  content += "function myFunction() {var d = new Date();var n = ~~(Date.now()/1000);document.getElementById(\"gmt\").value =n;";
  content += " document.getElementById(\"frm1\").submit();}</script>";
  content += "</body></html>";
  serverweb.send(200, "text/html", content);

}

void handlePark(void)
{ time_t now;
  now = time(nullptr);
  mount_park(telescope);
  String content =  "<html><style>" + String(BUTT) + String(TEXTT) + "</style>"+String(AUTO_SIZE)+"<body  bgcolor=\"#000000\" text=\"#FF6000\"><h2>ESP-PGT++ PARKED</h2><br>";
  content += "Mount parked  ,position saved on EEPROM.<br>";
  content += "AZ Counter:" + String(telescope->azmotor->counter) + "<br>";
  content += "Alt Counter:" + String(telescope->altmotor->counter) + "<br>";
  content += "TIme :" + String(ctime(&now)) + "<br>";
  content += "<button onclick=\"location.href='/'\"  type=\"button\">Back</button><br>";
  content += "</body></html>";

  serverweb.send(200, "text/html", content);
}
void handleRestart(void)
{ mount_park(telescope);
  String content =   "<html><body  bgcolor=\"#000000\" text=\"#FFFFFF\"><h2>ESP-PGT++ restarted</h2><br>";
  content += "Mount parked  ,position saved on EEPROM.<br>";
  content += "AZ Counter:" + String(telescope->azmotor->counter) + "<br>";
  content += "Alt Counter:" + String(telescope->altmotor->counter) + "<br>";
  content += "</body></html>";
  serverweb.send(200, "text/html", content);
  ESP.restart();
}

void handleNetwork( void) {
  String msg, ip, mask, gate, dns;
  if (serverweb.hasArg("IP") && serverweb.hasArg("MASK") && serverweb.hasArg("GATEWAY") && serverweb.hasArg("DNS"))
  {
    String net = serverweb.arg("IP") + "\n" + serverweb.arg("MASK") + "\n" + serverweb.arg("GATEWAY") + "\n" + serverweb.arg("DNS") + "\n";
    File f = SPIFFS.open("/network.config", "w");
    if (!f)
    {
      net = ("file open failed");
    }
    else
      f.println(net);
    f.close ();
    msg = serverweb.arg("IP");
    msg += "\n" + serverweb.arg("MASK");
    msg += "\n" + serverweb.arg("GATEWAY");
    msg += "\n" + serverweb.arg("DNS") + "\n";;
  }
  String content = "<html><style>" + String(BUTT) + String(TEXTT) + "</style>"+String(AUTO_SIZE)+"<body  bgcolor=\"#000000\" text=\"#FF6000\"><form action='/network' method='POST'><h2>Network Config</h2>";
  content += "<fieldset style=\"width:15%;border-radius:15px\"><legend>Network</legend><table style='width:200px'>";
  content += "<tr><td>IP</td><td><input type='text' name='IP' class=\"text_red\" value='" + WiFi.localIP().toString() + "'></td></td>";
  content += "<td><td>MASK</td><td><input type='test' name='MASK'class=\"text_red\"  value='" + WiFi.subnetMask().toString() + "'></td></tr>";
  content += "<tr><td>Gateway</td><td><input type='text' name='GATEWAY' class=\"text_red\" value='" + WiFi.gatewayIP().toString() + "'></td></td>";
  content += "<td><td>DNS</td><td><input type='test' name='DNS' class=\"text_red\"  value='" + WiFi.gatewayIP().toString() + "'></td></tr></table>";
  content += "<input type='submit' name='SUBMIT'  class=\"button_red\" value='Save'></fieldset></form>" + msg + "<br>";
  content += "<button onclick=\"location.href='/'\"class=\"button_red\" type=\"button\">Back</button><br>";
  content += "You must restart device for network changes to take effect";
  content += "</body></html>";

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
  serverweb.on("/", handleConfig);
  serverweb.on("/time", handleTime);
  serverweb.on("/sync", handleSync);
  serverweb.on("/park", handlePark);
  serverweb.on("/restart", handleRestart);
  serverweb.on("/network", handleNetwork);
  // serverweb.on("/formatfilesystem",handleFormat)
  serverweb.onNotFound([]()
  {
    if (!handleFileRead(serverweb.uri()))
      serverweb.send(404, "text/plain", "FileNotFound");
  });
  serverweb.begin();
}
