#include "WcsHandler.h"
#include "../Logger/Logger.h"
#include "../Network/TCP/TCP.h"
#include "../Network/Wifi/WifiNetwork.h"

// --------------------------
// Wcs Private Variables
// --------------------------
typedef enum
{
    NONE = 0,
    LOGIN,       // 1
    LOGOUT,      // 2
    PING,        // 3
    RETRIEVEBIN, // 4
    STOREBIN,    // 5
    MOVE,        // 6
    BATTERY,     // 7
    STATE,       // 8
    ERROR,
    Num_Of_WCS_Action_Enums
} ENUM_WCS_ACTIONS;
const char *STX = "\x02";
const char *ETX = "\x03";

// --------------------------
// Wcs Public Variables
// --------------------------
WcsHandler wcsHandler;

// --------------------------
// Wcs Private Methods
// --------------------------
int WcsHandler::read()
{
    // checks tcp socket for data to read
    char tempReadBuffer[DEFAULT_CHAR_ARRAY_SIZE];
    if (TcpRead(tempReadBuffer))
    {
        if (strlen(readBuffer) <= 0)
            strcpy(readBuffer, tempReadBuffer);
        else
            strcat(readBuffer, tempReadBuffer);

        // check buffer for end of instructions
        int pos = findIndex(readBuffer, ETX[0]);
        return pos;
    }
    return -1;
}

void WcsHandler::handle(int pos)
{
    // extracts the completed instructions from buffer
    // processes it and performs the instructions
    char handleBuffer[DEFAULT_CHAR_ARRAY_SIZE];
    cutStr(readBuffer, handleBuffer, 0, pos + 1);

    // remove stx and etx

    // interpret inputs

    // perform action
    info("Handled instructions");
    info(readBuffer);
    info(handleBuffer);
}

// --------------------------
// Wcs Public Methods
// --------------------------
void WcsHandler::init(void)
{
    ConnectWifi();
    info("Wifi connected");
    ConnectTcpServer();
    info("server connected");
}

void WcsHandler::run(void)
{
    int pos = this->read();
    if (pos >= 0)
        this->handle(pos);
}

bool WcsHandler::send(char *str)
{
    // 1    - STX
    // 2-5  - shuttle ID
    // 6-7  - shuttle action
    // 8-x  - other information encoded to enum
    // x+1  - ETX

    static char writeString[DEFAULT_CHAR_ARRAY_SIZE * 3];
    strcpy(writeString, STX);
    strcat(writeString, str);
    strcat(writeString, ETX);
    bool res = TcpWrite(writeString);

    writeString[0] = '\0';

    if (!res)
        return false;
    return true;
}