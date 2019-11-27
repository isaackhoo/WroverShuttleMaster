#include "WcsHandler.h"
#include "../Logger/Logger.h"
#include "../Network/TCP/TCP.h"
#include "../Network/Wifi/WifiNetwork.h"
#include "../Status/Status.h"
#include "../SlaveHandler/SlaveHandler.h"

// --------------------------
// Wcs Public Variables
// --------------------------
WcsHandler wcsHandler;

// --------------------------
// Wcs Private Methods
// --------------------------
int WcsHandler::hasCompleteInstructions()
{
    // checks readBuffer for completed instructions
    if (strlen(readBuffer) <= 0)
        return -1;
    return findIndex(readBuffer, ETX[0]);
};

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
        return this->hasCompleteInstructions();
    }
    return -1;
}

bool WcsHandler::interpret(char *input)
{
    // expected format: iiiiaax*
    // iiii = id
    // aa = wcsAction
    // x* = instructions of varied length

    // copy over input first since manipulation is required
    char copyInput[DEFAULT_CHAR_ARRAY_SIZE];
    strcpy(copyInput, input);

    if (strlen(copyInput) < DEFAULT_ID_LENGTH + DEFAULT_ACTION_ENUM_LENGTH)
        return false;
    // get id
    cutStr(copyInput, this->wcsIn.id, 0, DEFAULT_ID_LENGTH);
    // get action in string
    cutStr(copyInput, this->wcsIn.actionEnum, 0, DEFAULT_ACTION_ENUM_LENGTH);
    // interpret action
    this->wcsIn.action = (ENUM_WCS_ACTIONS)atoi(this->wcsIn.actionEnum);
    // get remaining instructions
    if (strlen(copyInput) > 0)
        strcpy(this->wcsIn.instructions, copyInput);

    // set information on shuttle status as well
    status.setWcsInputs(this->wcsIn.actionEnum, this->wcsIn.instructions);
};

void WcsHandler::perform()
{
    // takes information from received struct to perform necessary actions
    switch (this->wcsIn.action)
    {
    case LOGIN:
    {
        info("REC WCS::LOGIN");
        status.setId(this->wcsIn.id);
        break;
    }
    case LOGOUT:
    {
        info("REC WCS::LOGOUT");
        // do nothing for now
        break;
    }
    case PING:
    {
        // no need to edit anything,
        // just reply ping
        this->wcsOut = this->wcsIn;
        this->send();
        break;
    }
    case RETRIEVEBIN:
    {
        info("REC WCS::RETRIEVAL");
        slaveHandler.createRetrievalSteps(this->wcsIn.instructions);
        slaveHandler.beginNextStep();
        break;
    }
    case STOREBIN:
    {
        info("REC WCS::STORAGE");
        slaveHandler.createStorageSteps(this->wcsIn.instructions);
        slaveHandler.beginNextStep();
        break;
    }
    case MOVE:
    {
        info("REC WCS::MOVE");
        slaveHandler.createMovementSteps(this->wcsIn.instructions);
        slaveHandler.beginNextStep();
        break;
    }
    case BATTERY:
    {
        info("REC WCS::BATTERY");
        break;
    }
    case STATE:
    {
        info("REC WCS::STATE");
        break;
    }
    case ERROR:
    {
        break;
    }
    default:
    {
        break;
    }
    }
};

void WcsHandler::handle(int pos)
{
    // extracts the completed instructions from buffer
    // processes it and performs the instructions
    char handleBuffer[DEFAULT_CHAR_ARRAY_SIZE];
    cutStr(readBuffer, handleBuffer, 0, pos + 1);

    // remove stx and etx
    char pureInput[DEFAULT_CHAR_ARRAY_SIZE];
    cutStr(handleBuffer, pureInput, 1, strlen(handleBuffer) - 2);

    // interpret inputs
    bool interpretRes = this->interpret(pureInput);
    if (!interpretRes)
        return;

    // perform action
    this->perform();
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

bool WcsHandler::send()
{
    // WcsHandler::send overload.
    // compiles wcsOut into a cstring to send
    char wcsOutString[DEFAULT_CHAR_ARRAY_SIZE];

    strcpy(wcsOutString, this->wcsOut.id);
    strcat(wcsOutString, this->wcsOut.actionEnum);
    if (strlen(this->wcsOut.instructions) > 0)
        strcat(wcsOutString, this->wcsOut.instructions);
    this->send(wcsOutString);
};

void WcsHandler::pullCurrentStatus()
{
    // retrieves status information
    strcpy(this->wcsOut.id, status.getId());
};

// --------------------------
// Wcs Public Methods
// --------------------------
void WcsHandler::init(void)
{
    info("Connecting to Wifi");
    bool wifiConnectionRes = ConnectWifi();
    if (!wifiConnectionRes)
    {
        logSd("Failed to connect to wifi.");
        ESP.restart();
    }
    info("Wifi connected");
    info("Connecting to WCS Server");
    bool tcpConnectionRes = ConnectTcpServer();
    if (!tcpConnectionRes)
    {
        logSd("Failed to connect to server.");
        ESP.restart();
    }
    info("server connected");
    // retrieve existing shuttle status
    this->pullCurrentStatus();
    // login to server
    char loginEnumString[3];
    GET_TWO_DIGIT_STRING(loginEnumString, LOGIN);
    strcpy(this->wcsOut.actionEnum, loginEnumString);
    this->send();
}

void WcsHandler::run(void)
{
    int pos = this->read();
    while (pos > 0)
    {
        this->handle(pos);
        pos = hasCompleteInstructions();
    }
}

bool WcsHandler::sendJobCompletionNotification(const char *actionEnum, const char *inst)
{
    strcpy(this->wcsOut.id, status.getId());
    strcpy(this->wcsOut.actionEnum, actionEnum);
    if (strlen(inst) > 0)
        strcpy(this->wcsOut.instructions, inst);
    this->send();
};

bool WcsHandler::updateStateChange()
{
    strcpy(this->wcsOut.id, status.getId());
    char stateActionString[3];
    GET_TWO_DIGIT_STRING(stateActionString, STATE);
    strcpy(this->wcsOut.actionEnum, stateActionString);
    char currentStateString[3];
    GET_TWO_DIGIT_STRING(currentStateString, status.getState());
    strcpy(this->wcsOut.instructions, currentStateString);
    this->send();
};