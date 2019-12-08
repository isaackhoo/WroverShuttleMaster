#include <WiFi.h>
#include "WifiNetwork.h"

// -----------------------------
// NETWORK METHODS DEFINITION
// -----------------------------
bool ConnectWifi()
{
    WiFi.begin(ssid, password);
    unsigned long connectionTimeout = 10 * 1000; // 10s
    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - startTime > connectionTimeout)
        {
            return false;
        }
    }

    WiFi.onEvent([](WiFiEvent_t wifiEvent, WiFiEventInfo_t wifiEventInfo) {
        // attempt to reconnect on disconnection
        ConnectWifi();
    },
                 WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    return true;
};

bool isWifiConnected()
{
    if (WiFi.status() != WL_CONNECTED)
        return false;
    return true;
}