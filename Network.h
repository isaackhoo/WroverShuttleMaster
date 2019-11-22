#ifndef NETWORK_H
#define NETWORK_H
// -----------------------------
// NETWORK VARIABLE DECLARATION
// -----------------------------
extern const char* ssid;
extern const char* password;

extern const char* serverIp;
extern const int serverPort;

// -----------------------------
// NETWORK METHODS DECLARATION
// -----------------------------
extern void ConnectWifi();
extern bool isWifiConnected();
extern char* getCurrentDate();
extern char* getCurrentTime();

extern bool ConnectTcpServer();
extern char* TcpRead();
extern bool TcpWrite(char*);

#endif