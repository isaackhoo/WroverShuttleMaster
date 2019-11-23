#include "TCP.h"
#include "WString.h"
#include "../../Helper/Helper.h"

// ----------------------------
// TCP Client Variables
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
        String input = client.readString();
        strcpy(received, input.c_str());
        return true;
    }
    return false;
};

bool TcpWrite(char *toWrite)
{
    if (strlen(toWrite) <= 0)
        return false;
    if (!client.connected())
        return false;
    client.print(toWrite);
    return true;
};
