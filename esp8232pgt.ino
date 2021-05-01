#include "conf.h"
#include <time.h>
#include <Ticker.h>
#include <Wire.h>
#include "misc.h"
#include "piclevel.h"
#include "mount.h"
#include "webserver.h"
#ifdef OTA
#include <ArduinoOTA.h>
#include "OTA_helper.hpp"
#endif
#ifdef   IR_CONTROL
#include "ir_control.h"
#endif
#ifdef  NUNCHUCK_CONTROL
#include "nunchuck.h"
#endif
#include <FS.h>
//comment wifipass.h and uncomment for your  wifi parameters
//#include "wifipass.h"
const char* ssid = "MyWIFI";
const char* password = "Mypassword";
WiFiServer server(SERVER_PORT);
WiFiClient serverClients[MAX_SRV_CLIENTS];

#ifdef esp8266
ESP8266WebServer serverweb(WEB_PORT);
ESP8266HTTPUpdateServer httpUpdater;
#else
BluetoothSerial SerialBT;
WebServer serverweb(WEB_PORT);
//UpdateServer httpUpdater;
#endif
char buff[50] = "Waiting for connection..";
extern char  response[200];
byte napt=0;
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
//#include "SSD1306Wire.h"
//#include "SH1106.h"

#include "oled.h"
//SSD1306
//SSD1306Wire display(0x3c, D5, D6);

//SSD1306Wire display(0x3c, 2, 0);

#endif
#ifndef esp8266
void bttask(void)
{
    if (SerialBT.available())
    {
        char n = 0;
        while (SerialBT.available())  buff[n++] = (char) SerialBT.read() ;
        command(buff);
        buff[n] = 0;
        SerialBT.write((const uint8_t* )response, strlen(response));

    }
}
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
        ssi.trim();
        pwd.trim();
        WiFi.begin(ssi.c_str(), pwd.c_str());
    }
    else
    {
        WiFi.begin(ssid, password);
        SPIFFS.format();
    }
    f = SPIFFS.open("/network.config", "r");
    if (f)
    {
        IPAddress ip;
        IPAddress gateway;
        IPAddress subnet;
        IPAddress dns;
        if (ip.fromString(f.readStringUntil('\n')) &&
                subnet.fromString(f.readStringUntil('\n')) &&
                gateway.fromString(f.readStringUntil('\n')) &&
                dns.fromString(f.readStringUntil('\n')))
        {
            WiFi.config(ip, gateway, subnet, dns);
            napt=f.readStringUntil('\n').toInt();
        }

        f.close();
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
    if (WiFi.status() != WL_CONNECTED) WiFi.disconnect(true);else
    { dhcps_set_dns(1,WiFi.gatewayIP());
      dhcps_set_dns(0,WiFi.dnsIP(0));
      err_t ret = ip_napt_init(NAPT, NAPT_PORT);
    if (ret == ERR_OK) {
    ret = ip_napt_enable_no(SOFTAP_IF, napt);}
    }
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
#ifdef esp8266
    httpUpdater.setup(&serverweb);
#endif
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
#ifdef IR_CONTROL
    ir_init();
#endif
}

void loop()
{
    delay(10);
    net_task();
#ifndef esp8266
    bttask();
#endif
    now = time(nullptr);
    serverweb.handleClient();
#ifdef IR_CONTROL
    ir_read();
#endif
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
