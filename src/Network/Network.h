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
extern bool ConnectWifi();
extern bool isWifiConnected();
extern void getCurrentDate(char*);
extern void getCurrentTime(char*);

extern bool ConnectTcpServer();
extern bool TcpRead(char*);
extern bool TcpWrite(char*);

#endif