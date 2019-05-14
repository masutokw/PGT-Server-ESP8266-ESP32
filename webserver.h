#include <ESP8266WebServer.h>

#ifndef WEBSERVER_H_INCLUDED
#define WEBSERVER_H_INCLUDED
#include <ESP8266WebServer.h>
#include "mount.h"
#include <FS.h>
#define N_STYLE  " style='text-align: right;height:20px; width:60px' value ='"
extern ESP8266WebServer serverweb;
extern mount_t *telescope;
extern String ssi;
extern String pwd;
//void handleConfig();
//String getContentType(String filename);
//bool handleFileRead(String path);
void initwebserver(void);

#endif // WEBSERVER_H_INCLUDED
