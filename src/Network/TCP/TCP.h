#pragma once

#ifndef TCP_H
#define TCP_H

#include <WiFiClient.h>

// --------------------------
// TCP Client Variables
// --------------------------
extern WiFiClient client;

extern const char *serverIp;
extern const int serverPort;

// --------------------------
// TCP Client Public Methods
// --------------------------
bool ConnectTcpServer();
bool TcpRead(char *);
bool TcpWrite(const char *);

#endif