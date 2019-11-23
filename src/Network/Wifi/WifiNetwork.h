#ifndef WIFINETWORK_H
#define WIFINETWORK_H
// -----------------------------
// NETWORK VARIABLE DECLARATION
// -----------------------------
extern const char* ssid;
extern const char* password;

// -----------------------------
// NETWORK METHODS DECLARATION
// -----------------------------
extern bool ConnectWifi();
extern bool isWifiConnected();
extern void getCurrentDate(char*);
extern void getCurrentTime(char*);

#endif