#include <time.h>
#include <WiFi.h>
#include "WifiNetwork.h"
#include "../../Helper/Helper.h"

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

    WiFi.onEvent([](WiFiEvent_t wifiEvent, WiFiEventInfo_t wifiEventInfo){
        // attempt to reconnect on disconnection
        ConnectWifi();
    }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    return true;
};

bool isWifiConnected()
{
    if (WiFi.status() != WL_CONNECTED)
        return false;
    return true;
}

void getCurrentDate(char *output)
{
    if (WiFi.status() != WL_CONNECTED)
        ConnectWifi();
    long timezone = 8;
    byte daysavetime = 1;
    configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
    struct tm tmstruct;
    delay(2000); // wait for server response
    tmstruct.tm_year = 0;
    getLocalTime(&tmstruct, 5000); // 5 seconds time out to get local time

    char yearbuf[5];
    sprintf(yearbuf, "%d", tmstruct.tm_year + 1900);
    char monthbuf[3];
    GET_TWO_DIGIT_STRING(monthbuf, tmstruct.tm_mon + 1);
    char daybuf[3];
    GET_TWO_DIGIT_STRING(daybuf, tmstruct.tm_mday);

    strcpy(output, yearbuf);
    strcat(output, "_");
    strcat(output, monthbuf);
    strcat(output, "_");
    strcat(output, daybuf);
};

void getCurrentTime(char *output)
{
    if (WiFi.status() != WL_CONNECTED)
        ConnectWifi();
    long timezone = 8;
    byte daysavetime = 1;
    configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
    struct tm tmstruct;
    delay(2000); // wait for server response
    tmstruct.tm_year = 0;
    getLocalTime(&tmstruct, 5000); // 5 seconds time out to get local time

    char hourbuf[3];
    GET_TWO_DIGIT_STRING(hourbuf, tmstruct.tm_hour);
    char minutebuf[3];
    GET_TWO_DIGIT_STRING(minutebuf, tmstruct.tm_min);
    char secondsbuf[3];
    GET_TWO_DIGIT_STRING(secondsbuf, tmstruct.tm_sec);

    strcpy(output, hourbuf);
    strcat(output, ":");
    strcat(output, minutebuf);
    strcat(output, ":");
    strcat(output, secondsbuf);
}