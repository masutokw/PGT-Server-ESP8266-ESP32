#include <ESP8266WiFi.h>
#include <time.h>
#include "sntp.h"
#include <Ticker.h>
#include <Wire.h>
#include "SSD1306.h"
#include "misc.h"
#include "piclevel.h"
#include "mount.h"
#define BAUDRATE 19200
#define MAX_SRV_CLIENTS 1
#define SPEED_CONTROL_TICKER 10
#define COUNTERS_POLL_TICKER 150

#include "wifipass.h"
//const char* ssid = "MyWIFI";
//const char* password = "Mypassword";
extern picmsg  msg;
WiFiServer server(10001);
WiFiClient serverClients[MAX_SRV_CLIENTS];
char buff[50] = "E";
uint8_t sbuf[50];
extern char  response[200];
mount_t *mount_test;
Ticker speed_control_tckr,counters_poll_tkr;
extern long command( char *str );
time_t now;
SSD1306  display(0x3c, D5, D6);


int net_task(void)
{
    int lag=0;
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
                    buff[n]=0;
                    serverClients[i].write((char*)response,strlen(response));
                    //write some information for debugin purpose to OLED display.
                    display.clear();
                    display.drawString(0,0, String(buff));
                    display.drawString(0,15,String(response));
                    display.drawString(0,45,ctime(&now));
                    display.drawString(0,25,String(mount_test->azmotor->counter));
                    display.drawString(0,35,String(mount_test->altmotor->counter));
                    display.display();
                    //checkfsm();
                }

            }
        }
    }

}


void setup()
{
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.clear();
    display.drawString(0, 0, "Connecting to "+String(ssid));
    display.display();


    WiFi.begin(ssid, password);
    delay(500);
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
    if(i == 21)
    {

        while(1) delay(500);
    }
  //  display.clear();
    display.drawString(0, 0, "Connecting to "+String(ssid));
    display.drawString(0, 13, "Got IP! :"+String(WiFi.localIP()));
    display.drawString(0, 26, "Waiting for Client");
    display.display();
    config_NTP(1,0);
//start UART and the server
    Serial.begin(BAUDRATE);
    Serial.swap();
    server.begin();
    server.setNoDelay(true);
    mount_test=create_mount();
    mount_test->azmotor->speed=0.0;
    mount_test->altmotor->speed=0.0;
    mount_test->azmotor->targetspeed=0;//5.042*SEC_TO_RAD;
    mount_test->altmotor->targetspeed=0;//5.042*SEC_TO_RAD;


    speed_control_tckr.attach_ms(SPEED_CONTROL_TICKER,thread_motor,mount_test);
    counters_poll_tkr.attach_ms(COUNTERS_POLL_TICKER,thread_counter,mount_test);
}

void loop()
{
     delay(300-net_task());
     now   = time(nullptr);
 }




