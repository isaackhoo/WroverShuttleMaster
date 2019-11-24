#pragma once

#ifndef TCP_H
#define TCP_H

#include <WiFi.h>

// --------------------------
// TCP Client Variables
// --------------------------
extern WiFiClient client;

extern const char* serverIp;
extern const int serverPort;

// --------------------------
// TCP Client Public Methods
// --------------------------
extern bool ConnectTcpServer();
extern bool TcpRead(char*);
extern bool TcpWrite(char*);

#endif