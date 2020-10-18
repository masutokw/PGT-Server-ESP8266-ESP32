#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <time.h>
#include "sntp.h"
#include <Ticker.h>
#include <Wire.h>
#include "misc.h"
#include "piclevel.h"
#include "mount.h"
#include "webserver.h"
#define OTA
#ifdef OTA
#include <ArduinoOTA.h>
#include "OTA_helper.hpp"
#endif
//#define OLED_DISPLAY
//#define PAD
//Comment out undesired Feature
//---------------------------
#define NUNCHUCK_CONTROL
//#define FIXED_IP 17
//--------------------------------
#ifdef  NUNCHUCK_CONTROL
#include "nunchuck.h"
#endif

#define BAUDRATE 19200
#define MAX_SRV_CLIENTS 3
#define SPEED_CONTROL_TICKER 10
#define COUNTERS_POLL_TICKER 100
#include <FS.h>

//comment wifipass.h and uncomment for your  wifi parameters
//#include "wifipass.h"
const char* ssid = "MyWIFI";
const char* password = "Mypassword";
extern picmsg  msg;
extern volatile int state;
WiFiServer server(10001);
WiFiClient serverClients[MAX_SRV_CLIENTS];
ESP8266WebServer serverweb(80);
ESP8266HTTPUpdateServer httpUpdater;
char buff[50] = "Waiting for connection..";
extern char  response[200];
mount_t *telescope;
String ssi;
String pwd;
Ticker speed_control_tckr, counters_poll_tkr;

extern long command( char *str );
time_t now;
#ifdef PAD
#include "pad.h"
#endif
#ifdef OLED_DISPLAY
#include "SSD1306Wire.h"
//#include "SH1106.h"

//#include "pad.h"
//SSD1306
SSD1306Wire display(0x3c, D5, D6);



#endif


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
  return millis() - lag;
}

void setup()
{

#ifdef OLED_DISPLAY
  oled_initscr();



#endif

#ifdef NUNCHUCK_CONTROL
  // nunchuck_init(D6, D5);
  nunchuck_init(2, 0);

#endif

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("PGT_ESP", "boquerones");
  SPIFFS.begin();
  File f = SPIFFS.open("/wifi.config", "r");
  if (f)
  {
    ssi = f.readStringUntil('\n');
    pwd = f.readStringUntil('\n');
    f.close();
    char  ss [ssi.length() + 1];
    char  pw [pwd.length() + 1];
    ssi.toCharArray(ss, ssi.length() + 1);
    pwd.toCharArray(pw, pwd.length() + 1);
    pw[pwd.length() + 1] = 0;
    ss[ssi.length() + 1] = 0;

    WiFi.begin((const char*)ss, (const char*)pw);
  }
  else  {
    WiFi.begin(ssid, password);
    SPIFFS.format();
  }
#ifdef FIXED_IP
  IPAddress ip(192, 168, 1, FIXED_IP);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 254, 0);
  IPAddress DNS(192, 168, 1, 1);
  WiFi.config(ip, gateway, subnet, gateway);
#endif

  delay(500);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if (i == 21)
  {
    //     while (1) delay(500);
  }
  if (WiFi.status() != WL_CONNECTED) WiFi.disconnect(true);
#ifdef OLED_DISPLAY
  oled_waitscr();
#endif

  //start UART and the server
  Serial.begin(BAUDRATE);
#ifdef OLED_DISPLAY
  // Serial.swap();
#endif
  //
  server.begin();
  server.setNoDelay(true);
  telescope = create_mount();
  readconfig(telescope);
  httpUpdater.setup(&serverweb);
  config_NTP(telescope->time_zone, 0);
  initwebserver();
  delay (2000) ;
  sdt_init(telescope->longitude, telescope->time_zone);
  speed_control_tckr.attach_ms(SPEED_CONTROL_TICKER, thread_motor, telescope);
  counters_poll_tkr.attach_ms(COUNTERS_POLL_TICKER, thread_counter, telescope);
#ifdef PAD
  pad_Init();
#endif
#ifdef OTA
  InitOTA();
#endif
}

void loop()
{
  delay(10);
  net_task();
  now = time(nullptr);
  serverweb.handleClient();

#ifdef  NUNCHUCK_CONTROL
  nunchuck_read() ;
#endif
#ifdef PAD
  doEvent();
#endif
#ifdef OLED_DISPLAY
  oledDisplay();
#endif
#ifdef OTA
  ArduinoOTA.handle();
#endif
}
