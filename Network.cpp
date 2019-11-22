#include <time.h>
#include <WiFi.h>

#include "Helper.h"
#include "Network.h"

// -----------------------------
// NETWORK VARIABLE DEFINITION
// -----------------------------
const char *ssid = "LSH-UNIFI-MAIN";
const char *password = "PowerLSH@@";

const char *serverIp = "192.168.201.45";
const int serverPort = 55555;

WiFiClient client;

// -----------------------------
// NETWORK METHODS DEFINITION
// -----------------------------
void ConnectWifi()
{
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        info(".");
    }
    info("Wifi Connected");
};

bool isWifiConnected()
{
    if (WiFi.status() != WL_CONNECTED)
        return false;
    return true;
}

char *getCurrentDate()
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

    static char output[11]; // yyyy/mm/dd\0

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

    return output;
};

char *getCurrentTime()
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

    static char output[9]; // hh:mm:ssid

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

    return output;
}

bool ConnectTcpServer()
{
    if (!client.connect(serverIp, serverPort))
    {
        info("Failed to connect to server");
        return false;
    }
    return true;
};

char *TcpRead()
{
    static char received[DEFAULT_CHAR_ARRAY_SIZE];
    if (client.available())
    {
        String input = client.readString();
        input.trim();
        // stringToCharArr(received, input);
    }
    return received;
};

bool TcpWrite(char write[]){

};
