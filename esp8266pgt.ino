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
#define MAX_SRV_CLIENTS 1
#define SPEED_CONTROL_TICKER 10
#define COUNTERS_POLL_TICKER 150
#include <FS.h>
#include "wifipass.h"
//#define OLED_DISPLAY
//const char* ssid = "MyWIFI";
//const char* password = "Mypassword";
extern picmsg  msg;
WiFiServer server(10001);
WiFiClient serverClients[MAX_SRV_CLIENTS];
ESP8266WebServer serverweb(80);
char buff[50] = "E";
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
    //write some information for debuging purpose to OLED display.
    display.clear();
    display.drawString (0, 0, "ESP-8266 PicGoto++ 0.1");
    display.drawString(0, 15, String(buff) + "  " + String(response));
    display.drawString(0, 45, ctime(&now));
    display.drawString(0, 25, "MA:" + String(telescope->azmotor->counter) + " MD:" + String(telescope->altmotor->counter));
    display.drawString(0, 35, "Dt:" + String(telescope->azmotor->slewing) + " Dt:" + String(telescope->azmotor->delta/ (SEC_TO_RAD)));
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
    int lag = 0;
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

}


void setup()
{

#ifdef OLED_DISPLAY
    oled_initscr();

#endif

    SPIFFS.begin();
    //WiFi.softAP("picgoto");
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
    config_NTP(1, 0);
    //start UART and the server
    Serial.begin(BAUDRATE);
#ifdef OLED_DISPLAY
    Serial.swap();
#endif
    //
    server.begin();
    server.setNoDelay(true);
    telescope = create_mount();
    telescope->azmotor->speed = 0.0;
    telescope->altmotor->speed = 0.0;
    telescope->azmotor->targetspeed = 0; //5.042*SEC_TO_RAD;
    telescope->altmotor->targetspeed = 0; //5.042*SEC_TO_RAD;

    serverweb.onNotFound([]()
    {
        if (!handleFileRead(serverweb.uri()))
            serverweb.send(404, "text/plain", "FileNotFound");
    });
    serverweb.on("/all", HTTP_GET, []()
    {
        String json = "[{";
        json += "\"MaxCounter\":" + String(telescope->azmotor->maxcounter);
        json += ",\"MaxCounter\":" + String(telescope->altmotor->maxcounter);
        json += ", \"analog\":" + String(analogRead(A0));
        json += ", \"RateG\":" + String(telescope->rate[0]);
        json += ", \"RateC\":" + String(telescope->rate[1]);
        json += ", \"RateF\":" + String(telescope->rate[2]);
        json += ", \"RateS\":" + String(telescope->rate[3]);
        json += ", \"Longitude\":" + String(telescope->longitude);
        json += "}]";
        serverweb.send(200, "text/json", json);
        json = String();
    });
    serverweb.begin();

    speed_control_tckr.attach_ms(SPEED_CONTROL_TICKER, thread_motor, telescope);
    counters_poll_tkr.attach_ms(COUNTERS_POLL_TICKER, thread_counter, telescope);
}

void loop()
{
    delay(60);
    net_task();
    now   = time(nullptr);
    //serverweb.handleClient();
#ifdef OLED_DISPLAY
    oledDisplay();
#endif

}




