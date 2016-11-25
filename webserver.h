#ifndef WEBSERVER_H_INCLUDED
#define WEBSERVER_H_INCLUDED
#include <ESP8266WebServer.h>
#include "mount.h"
#include <FS.h>

extern ESP8266WebServer serverweb;
extern mount_t *telescope;
//void handleConfig();
//String getContentType(String filename);
//bool handleFileRead(String path);
void initwebserver(void);

#endif // WEBSERVER_H_INCLUDED
