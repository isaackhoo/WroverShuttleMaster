#include <cstring>
#include "TCP.h"

// ----------------------------
// TCP Client Public Variables
// ----------------------------
WiFiClient client;

// --------------------------
// TCP Client Public Methods
// --------------------------
bool ConnectTcpServer()
{
    if (!client.connect(serverIp, serverPort))
        return false;
    return true;
};

bool TcpRead(char *received)
{
    if (client.available())
    {
        // does not cause heap fragmentation. tested
        String input = client.readString();
        input.trim();
        Serial.print("received ");
        Serial.println(input);

        if (strlen(received) == 0)
            strcpy(received, input.c_str());
        else
            strcat(received, input.c_str());
        return true;
    }
    return false;
};

bool TcpWrite(const char *toWrite)
{
    if (strlen(toWrite) <= 0)
        return false;
    if (!client.connected())
        return false;
    client.print(toWrite);
    return true;
};