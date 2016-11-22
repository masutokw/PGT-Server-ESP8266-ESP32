#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <time.h>
#include "sntp.h"
#include <Ticker.h>
#include <Wire.h>
#include "misc.h"
#include "piclevel.h"
#include "mount.h"

#define BAUDRATE 19200
#define MAX_SRV_CLIENTS 3
#define SPEED_CONTROL_TICKER 10
#define COUNTERS_POLL_TICKER 100
#include <FS.h>
#include "wifipass.h"
#define OLED_DISPLAY
//const char* ssid = "MyWIFI";
//const char* password = "Mypassword";
extern picmsg  msg;
WiFiServer server(10001);
WiFiClient serverClients[MAX_SRV_CLIENTS];
ESP8266WebServer serverweb(80);
char buff[50] = "Waiting for connection..";
uint8_t sbuf[50];
extern char  response[200];
mount_t *telescope;
Ticker speed_control_tckr, counters_poll_tkr;
extern long command( char *str );
time_t now;
#ifdef OLED_DISPLAY
#include "SSD1306.h"
SSD1306  display(0x3c, D5, D6);
void oledDisplay()
{
    char ra[20]="";
    char de[20]="";
    //write some information for debuging purpose to OLED display.
    display.clear();
// display.drawString (0, 0, "ESP-8266 PicGoto++ 0.1");
 //   display.drawString(0, 13, String(buff) + "  " + String(response));
 lxprintra(ra,sidereal_timeGMT_alt(telescope->longitude)*15.0*DEG_TO_RAD);
display.drawString(0, 13,"LST " + String(ra));
    lxprintra(ra,calc_Ra(telescope->azmotor->pos_angle,telescope->longitude));
    lxprintde(de,telescope->altmotor->pos_angle);

    display.drawString(0, 2,"RA:"+String(ra)+" DE:"+String(de));
    lxprintde(de,telescope->azmotor->delta);
    display.drawString(0, 42,String(de));// ctime(&now));
    display.drawString(0, 22, "MA:" + String(telescope->azmotor->counter) + " MD:" + String(telescope->altmotor->counter));
    display.drawString(0, 32, "Dt:" + String(telescope->azmotor->slewing));
    display.drawString(0, 52,ctime(&now));
    display.display();
}
void oled_initscr(void)

{
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.clear();
    display.drawString(0, 0, "Connecting to " + String(ssid));
    display.display();
}

void oled_waitscr(void)
{
    display.clear();
    display.drawString(0, 0, "Connecting to " + String(ssid));
    display.drawString(0, 13, "Got IP! :" + String(WiFi.localIP()));
    display.drawString(0, 26, "Waiting for Client");
    display.display();
}

#endif

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

    if (serverweb.hasArg("USERNAME") && serverweb.hasArg("PASSWORD"))
    {
        if (serverweb.arg("USERNAME") == "admin" &&  serverweb.arg("PASSWORD") == "admin" )
        {
            String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=1\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
            serverweb.sendContent(header);
            Serial.println("Log in Successful");
            return;
        }
        msg = "Values:"+serverweb.arg("MAXCOUNTER");
        msg+="\r"+serverweb.arg("MAXCOUNTER_ALT");
        msg+="\r"+serverweb.arg("GUIDE");
        msg+="\r"+serverweb.arg("CENTER");
        msg+="\r "+serverweb.arg("FIND");
        msg+="\r"+serverweb.arg("SLEW");
        msg+="\r"+serverweb.arg("LATITUDE");
        msg+="\r"+serverweb.arg("LONGITUDE");
        String temp=serverweb.arg("SLEW");

        telescope->rate[3]=temp.toFloat();
        Serial.println("Log in Failed");
    }
    String content = "<html><body><form action='/config' method='POST'><h2>ESP-PGT++ Config</h2><br>";
    content+="<fieldset> <legend>Login  information:</legend>";
    content+= "User:<br><input type='text' name='USERNAME' placeholder='user name'><br>";
    content+= "Password:<br><input type='password' name='PASSWORD' placeholder='password'><br></fieldset>";
    content+="<fieldset> <legend>Mount parameters:</legend>";
    content+= "RA steps/rev:<br><input type='number' width='48' name='MAXCOUNTER' placeholder='Gear' value ='" + String(telescope->azmotor->maxcounter) + "'><br>";
    content+= "DEC steps/rev:<br><input type='number' name='MAXCOUNTER_ALT' placeholder='Gear' value ='" + String(telescope->altmotor->maxcounter) + "'><br>";
    content+= "Guide rate:<br><input type='number' name='GUIDE' placeholder='GUIDE' value ='" + String(telescope->rate[0]) + "'><br>";
    content+= "Center rate:<br><input type='number' name='CENTER' placeholder='CENTER' value ='" + String(telescope->rate[1]) + "'><br>";
    content+= "Find rate:<br><input type='number' name='FIND' placeholder='FIND' value ='" + String(telescope->rate[2]) + "'><br>";
    content+= "Slew rate:<br><input type='number' name='SLEW' placeholder='SLEW' value ='" + String(telescope->rate[3]) + "'><br></fieldset>";
    content+="<fieldset> <legend>Geodata</legend>";
    content+= "Longitude:<br><input type='number' name='LONGITUDE' placeholder='Long' value ='" + String(telescope->longitude) + "'><br>";
    content+= "Latitude:<br><input type='number' name='LATITUDE' placeholder='Lat' value ='" + String(telescope->lat) + "'><br>";
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

int net_task(void)
{
    int lag = millis();
    size_t n;
    uint8_t i;
    //Sky Safari does not make a persistent connection, so each commnad query is managed as a single independent client.
    if (server.hasClient())
    {
        for (i = 0; i < MAX_SRV_CLIENTS; i++)
        {
            //find free/disconnected spot
            if (!serverClients[i] || !serverClients[i].connected())
            {
                if (serverClients[i]) serverClients[i].stop();
                serverClients[i] = server.available();
                continue;
            }
        }
        //Only one client at time, so reject
        WiFiClient serverClient = server.available();
        serverClient.stop();
    }
    //check clients for data
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
    {
        if (serverClients[i] && serverClients[i].connected())
        {
            if (serverClients[i].available())
            {
                //get data from the  client and push it to LX200 FSM

                while (serverClients[i].available())
                {
                    delay(1);
                    size_t n = serverClients[i].available();
                    serverClients[i].readBytes(buff, n);
                    command( buff);
                    buff[n] = 0;
                    serverClients[i].write((char*)response, strlen(response));

                    //checkfsm();
                }

            }
        }
    }
    return millis()-lag;
}



void setup()
{

#ifdef OLED_DISPLAY
    oled_initscr();

#endif

    SPIFFS.begin();
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("PGT_ESP","potatoe");
    WiFi.begin(ssid, password);
    delay(500);
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
    if (i == 21)
    {

        while (1) delay(500);
    }
#ifdef OLED_DISPLAY
    oled_waitscr();
#endif

    //start UART and the server
    Serial.begin(BAUDRATE);
#ifdef OLED_DISPLAY
    Serial.swap();
#endif
    //
    server.begin();
    server.setNoDelay(true);
    telescope = create_mount();
    config_NTP(telescope->time_zone, 0);
    serverweb.on("/config", handleConfig);
    serverweb.onNotFound([]()
    {
        if (!handleFileRead(serverweb.uri()))
            serverweb.send(404, "text/plain", "FileNotFound");
    });
    serverweb.begin();
    delay (2000) ;sdt_init(telescope->longitude,telescope->time_zone);
    speed_control_tckr.attach_ms(SPEED_CONTROL_TICKER, thread_motor, telescope);
    counters_poll_tkr.attach_ms(COUNTERS_POLL_TICKER, thread_counter, telescope);
}

void loop()
{
    delay(10);
    net_task();
    now   = time(nullptr);
    serverweb.handleClient();
#ifdef OLED_DISPLAY
    oledDisplay();
#endif

}




