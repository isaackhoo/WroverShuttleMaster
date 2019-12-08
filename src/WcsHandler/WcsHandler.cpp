#include <string.h>
#include <stdio.h>
#include "WcsHandler.h"
#include "../Network/Wifi/WifiNetwork.h"
#include "../Network/TCP/TCP.h"
#include "../Helper/helper.h"

// --------------------------
// Wcs Public Variables
// --------------------------
WcsHandler wcsHandler;

// --------------------------
// Wcs Public Methods
// --------------------------
bool WcsHandler::interpret(char *input)
{
    // expected format: iiiiaax*
    // iiii = id
    // aa = wcsAction
    // x* = instructions of varied length

    // copy over input first since manipulation is required
    char copyInput[128];
    // strcpy_s(copyInput, sizeof copyInput, input);
    strcpy(copyInput, input);

    if (strlen(copyInput) < 4 + 2)
        return false;
    // get id
    strcut(this->wcsIn.id, copyInput, 0, 4);
    // get action in string
    strcut(this->wcsIn.actionEnum, copyInput, 0, 2);
    // interpret action
    this->wcsIn.action = (ENUM_WCS_ACTIONS)atoi(this->wcsIn.actionEnum);

    // clear instructions
    this->wcsIn.instructions[0] = '\0';

    // get remaining instructions
    if (strlen(copyInput) > 0)
        // strcpy_s(this->wcsIn.instructions, sizeof this->wcsIn.instructions, copyInput);
        strcpy(this->wcsIn.instructions, copyInput);
};

void WcsHandler::perform()
{
    // takes information from received struct to perform necessary actions
    switch (this->wcsIn.action)
    {
    case LOGIN:
    {
        Serial.println("REC WCS::LOGIN");
        strcpy(this->wcsOut.id, this->wcsIn.id);
        break;
    }
    case LOGOUT:
    {
        Serial.println("REC WCS::LOGOUT");
        // do nothing for now
        break;
    }
    case PING:
    {
        // no need to edit anything,
        // just reply ping
        this->wcsOut = this->wcsIn;
        this->sendToServer();
        break;
    }
    case RETRIEVEBIN:
    {
        Serial.println("REC WCS::RETRIEVAL");
        char stateAction[3];
        GET_TWO_DIGIT_STRING(stateAction, STATE);
        strcpy(this->wcsOut.actionEnum, stateAction);

        char workingState[3];
        GET_TWO_DIGIT_STRING(workingState, RETRIEVING);
        strcpy(this->wcsOut.instructions, workingState);

        this->sendToServer();

        GET_TWO_DIGIT_STRING(workingState, IDLE);
        strcpy(this->wcsOut.instructions, workingState);

        this->sendToServer();
        break;
    }
    case STOREBIN:
    {
        Serial.println("REC WCS::STORAGE");
        char stateAction[3];
        GET_TWO_DIGIT_STRING(stateAction, STATE);
        strcpy(this->wcsOut.actionEnum, stateAction);

        char workingState[3];
        GET_TWO_DIGIT_STRING(workingState, STORING);
        strcpy(this->wcsOut.instructions, workingState);

        this->sendToServer();

        GET_TWO_DIGIT_STRING(workingState, IDLE);
        strcpy(this->wcsOut.instructions, workingState);

        this->sendToServer();
        break;
    }
    case MOVE:
    {
        Serial.println("REC WCS::MOVE");
        break;
    }
    case BATTERY:
    {
        Serial.println("REC WCS::BATTERY");
        break;
    }
    case STATE:
    {
        Serial.println("REC WCS::STATE");
        break;
    }
    case LEVEL:
    {
        Serial.println("REC WCS::LEVEL");
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

// --------------------------
// Wcs Private Methods
// --------------------------
void WcsHandler::init()
{
    if (!ConnectWifi())
        Serial.println("Failed to connect to wifi");
    Serial.println("Connected to wifi");

    if (!ConnectTcpServer())
        Serial.println("Failed to connect to server");
    Serial.println("Conencted to server");

    strcpy(this->wcsOut.id, "0000");
    char actionString[3];
    GET_TWO_DIGIT_STRING(actionString, LOGIN);
    strcpy(this->wcsOut.actionEnum, actionString);

    Serial.println("logging in to server");
    this->sendToServer();
}

bool WcsHandler::sendToServer(const char *output)
{
    char out[512];

    strcpy(out, STX);
    strcat(out, output);
    strcat(out, ETX);

    Serial.print("Sending to server: ");
    Serial.println(out);

    return TcpWrite(out);
};

bool WcsHandler::sendToServer()
{
    char wcsOutString[256];

    strcpy(wcsOutString, this->wcsOut.id);
    strcat(wcsOutString, this->wcsOut.actionEnum);
    if (strlen(this->wcsOut.instructions))
        strcat(wcsOutString, this->wcsOut.instructions);

    this->sendToServer(wcsOutString);
};

void WcsHandler::run()
{
    if (TcpRead(this->tcpReadBuff))
    {
        int headerIndex = findIndex(this->tcpReadBuff, STX[0]);
        int footerIndex = findIndex(this->tcpReadBuff, ETX[0]);

        if (footerIndex < 0)
        {
            Serial.println("timeout code");
        }
        else
        {
            while (headerIndex >= 0 && footerIndex > headerIndex)
            {
                // extract input
                char temp[128];
                strcut(temp, this->tcpReadBuff, headerIndex, footerIndex - headerIndex + 1);

                Serial.print("leftover in read buffer ");
                Serial.println(this->tcpReadBuff);

                // clear timeout
                // todo

                // remove stx and etx from input
                char cleanInput[128];
                strcut(cleanInput, temp, 1, strlen(temp) - 2);

                // interpret input
                Serial.print("intepreting ");
                Serial.println(cleanInput);
                this->interpret(cleanInput);

                // perform input
                Serial.println("performing");
                this->perform();

                // check for more inputs
                if (strlen(this->tcpReadBuff) > 0)
                {
                    headerIndex = findIndex(this->tcpReadBuff, STX[0]);
                    footerIndex = findIndex(this->tcpReadBuff, ETX[0]);
                }
                else
                {
                    headerIndex = -1;
                    footerIndex = -1;
                }
            }
        }
    }
}