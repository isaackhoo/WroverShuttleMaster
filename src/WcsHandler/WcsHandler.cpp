#include <string.h>
#include <stdio.h>
#include "WcsHandler.h"
#include "../Logger/Logger.h"
#include "../Logger/SD/SD.h"
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
            // strcpy_s(readBuffer, sizeof readBuffer, tempReadBuffer);
            strcpy(readBuffer, tempReadBuffer);
        else
            // strcat_s(readBuffer, sizeof readBuffer, tempReadBuffer);
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
    // strcpy_s(copyInput, sizeof copyInput, input);
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
        // strcpy_s(this->wcsIn.instructions, sizeof this->wcsIn.instructions, copyInput);
        strcpy(this->wcsIn.instructions, copyInput);

    // set information on shuttle status as well
    if ((ENUM_WCS_ACTIONS)atoi(this->wcsIn.actionEnum) != PING)
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
        status.saveStatus();
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
        this->send(false);
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
    case LEVEL:
    {
        info("REC WCS::LEVEL");
        status.setLevel(this->wcsIn.instructions);
        status.saveStatus();
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

bool WcsHandler::send(char *str, bool shouldLog = true)
{
    // 1    - STX
    // 2-5  - shuttle ID
    // 6-7  - shuttle action
    // 8-x  - other information encoded to enum
    // x+1  - ETX

    static char writeString[DEFAULT_CHAR_ARRAY_SIZE * 3];
    // strcpy_s(writeString, sizeof writeString, STX);
    // strcat_s(writeString, sizeof writeString, str);
    // strcat_s(writeString, sizeof writeString, ETX);

    strcpy(writeString, STX);
    strcat(writeString, str);
    strcat(writeString, ETX);
    bool res = TcpWrite(writeString);

    static char wcsSendLog[DEFAULT_CHAR_ARRAY_SIZE * 4];

    if (!res)
        sprintf(wcsSendLog, "Failed to send to server: %s", writeString);
    else
        sprintf(wcsSendLog, "Sent to server: %s", writeString);

    if (shouldLog)
        logSd(wcsSendLog);
    writeString[0] = '\0';
    return res;
}

bool WcsHandler::send(bool shouldLog = true)
{
    // WcsHandler::send overload.
    // compiles wcsOut into a cstring to send
    char wcsOutString[DEFAULT_CHAR_ARRAY_SIZE];

    // strcpy_s(wcsOutString, sizeof wcsOutString, this->wcsOut.id);
    // strcat_s(wcsOutString, sizeof wcsOutString, this->wcsOut.actionEnum);

    strcpy(wcsOutString, this->wcsOut.id);
    strcat(wcsOutString, this->wcsOut.actionEnum);
    if (strlen(this->wcsOut.instructions) > 0)
        // strcat_s(wcsOutString, sizeof wcsOutString, this->wcsOut.instructions);
        strcat(wcsOutString, this->wcsOut.instructions);
    this->send(wcsOutString, shouldLog);
};

void WcsHandler::pullCurrentStatus()
{
    // retrieves status information
    strcpy(this->wcsOut.id, status.getId());
    if (!status.isIdDefault())
    {
        strcpy(this->wcsOut.instructions, status.getLevel());
    } 
};

// --------------------------
// Wcs Public Methods
// --------------------------
void WcsHandler::init(void)
{
    // rehydration
    info("rehydrating status");
    status.rehydrateStatus(readStatus());
    info("rehydration complete");

    // wifi connection
    info("Connecting to Wifi");
    bool wifiConnectionRes = ConnectWifi();
    if (!wifiConnectionRes)
    {
        logSd("Failed to connect to wifi.");
        ESP.restart();
    }
    info("Wifi connected");

    // wcs connection
    info("Connecting to WCS Server");
    bool tcpConnectionRes = ConnectTcpServer();
    if (!tcpConnectionRes)
    {
        logSd("Failed to connect to server.");
        ESP.restart();
    }
    info("server connected");
    // retrieve existing shuttle id
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
    // strcpy_s(this->wcsOut.id, sizeof this->wcsOut.id, status.getId());
    // strcpy_s(this->wcsOut.actionEnum, sizeof this->wcsOut.actionEnum, actionEnum);

    strcpy(this->wcsOut.id, status.getId());
    strcpy(this->wcsOut.actionEnum, actionEnum);
    if (strlen(inst) > 0)
        // strcpy_s(this->wcsOut.instructions, sizeof this->wcsOut.instructions, inst);
        strcpy(this->wcsOut.instructions, inst);
    this->send();
};

bool WcsHandler::updateStateChange()
{
    // strcpy_s(this->wcsOut.id, sizeof this->wcsOut.id, status.getId());
    strcpy(this->wcsOut.id, status.getId());
    char stateActionString[3];
    GET_TWO_DIGIT_STRING(stateActionString, STATE);
    // strcpy_s(this->wcsOut.actionEnum, sizeof this->wcsOut.actionEnum, stateActionString);
    strcpy(this->wcsOut.actionEnum, stateActionString);
    char currentStateString[3];
    GET_TWO_DIGIT_STRING(currentStateString, status.getState());
    // strcpy_s(this->wcsOut.instructions, sizeof this->wcsOut.instructions, currentStateString);
    strcpy(this->wcsOut.instructions, currentStateString);
    this->send();
};