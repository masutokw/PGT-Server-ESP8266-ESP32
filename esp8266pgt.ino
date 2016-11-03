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
    //check if there are any new clients
    if (server.hasClient())
    {
        for (i = 0; i < MAX_SRV_CLIENTS; i++)
        {
            //find free/disconnected spot
            if (!serverClients[i] || !serverClients[i].connected())
            {
                if (serverClients[i]) serverClients[i].stop();
                serverClients[i] = server.available();
             //   Serial1.println("New client: ");
             //   Serial1.println(i);
                continue;
            }
        }
        //no free/disconnected spot so reject
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
                //get data from the  client and push it to the UART

                while (serverClients[i].available())
                {
                    delay(10); display.clear();
                    size_t n = serverClients[i].available();
                    serverClients[i].readBytes(buff, n);
                    command( buff);
                   // Serial.write(buff,n);
                   buff[n]=0;
                    display.drawString(0,0, String(buff)+" "+String(response));
                   // Serial.println();
                     display.drawString(0,15,ctime(&now));
                    serverClients[i].write((char*)response,strlen(response));
                   // Serial.write(response,strlen(response));
                    display.display();
                    // checkfsm();
                }

            }
        }
    }

}


void setup()
{  display.init();
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
     display.clear();
     display.drawString(0, 0, "Connecting to "+String(ssid));
     display.drawString(0, 13, "Got IP! :"+String(WiFi.localIP()));
     display.drawString(0, 26, "Waiting for Client");
     display.display();
    config_NTP(1,0);
//start UART and the server
    Serial.begin(BAUDRATE);Serial.swap();
    server.begin();
    server.setNoDelay(true);
    mount_test=create_mount();
    mount_test->azmotor->speed=0.0;
    mount_test->altmotor->speed=0.0;
    mount_test->azmotor->targetspeed=0;//5.042*SEC_TO_RAD;
    mount_test->altmotor->targetspeed=0;//5.042*SEC_TO_RAD;


    speed_control_tckr.attach_ms(50,thread_motor,mount_test);
    counters_poll_tkr.attach_ms(300,thread_counter,mount_test);
}

void loop()
{
 // display.drawString(0, 0, String(millis()));
  // write the buffer to the display
 // display.display();


// motor_set_period (0x30, 0x3132, 0x34); delay(1000);
// Serial.println();
    delay(300-net_task());
    //delay(10);

//mount_test->azmotor->targetspeed+=(1.0)*SEC_TO_RAD;
//mount_test->altmotor->targetspeed-=(0.1)*SEC_TO_RAD;
//Serial.print(mount_test->altmotor->speed,6);
//Serial.print("  ");
    // int p =millis();
//pollcounters(253);
//   Serial.println(mount_test->altmotor->counter);
//pollcounters(254);
//    Serial.println(mount_test->azmotor->counter);
    //Serial.println(Serial.available());
   // Serial.println(sidereal_timeGMT(-4.0),7 );
  now   = time(nullptr);
  //  Serial.println(ctime(&now));



}




