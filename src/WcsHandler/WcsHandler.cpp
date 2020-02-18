#include <string.h>
#include <stdio.h>
#include "WcsHandler.h"
#include "../Logger/Logger.h"
#include "../Logger/SD/SD.h"
#include "../Network/TCP/TCP.h"
#include "../Network/Wifi/WifiNetwork.h"
#include "../Status/Status.h"
#include "../SlaveHandler/SlaveHandler.h"
#include "../Logger/EEPROM/ELog.h"

// --------------------------
// Wcs Public Variables
// --------------------------
WcsHandler wcsHandler;

// --------------------------
// Wcs Private Methods
// --------------------------
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
    strcut(this->wcsIn.id, copyInput, 0, DEFAULT_ID_LENGTH);
    // get action in string
    strcut(this->wcsIn.actionEnum, copyInput, 0, DEFAULT_ACTION_ENUM_LENGTH);
    // interpret action
    this->wcsIn.action = (ENUM_WCS_ACTIONS)atoi(this->wcsIn.actionEnum);
    // clear instructions
    this->wcsIn.instructions[0] = '\0';
    // get remaining instructions
    if (strlen(copyInput) > 0)
        strcpy(this->wcsIn.instructions, copyInput);

    // set information on shuttle status as well

    if ((ENUM_WCS_ACTIONS)atoi(this->wcsIn.actionEnum) != PING && (ENUM_WCS_ACTIONS)atoi(this->wcsIn.actionEnum) != ECHO)
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
        status.saveStatus(); // forced to perform the save outside here, or wrover will not load up. No idea why

        // check eeprom for messages unsent, try resending it
        char *unrepliedEcho = readEEPROMMsg();
        clearEEPROMMsg();
        if (strlen(unrepliedEcho) > 0)
        {
            this->send(unrepliedEcho, true, true);
        }
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
        this->send(false, false);
        this->updateLastPing();
        break;
    }
    case RETRIEVEBIN:
    {
        info("REC WCS::RETRIEVAL");
#ifdef TCP_STRESS_TEST
        // set status active
        status.setState(RETRIEVING);

        delay(1000);

        // set status idle
        status.setState(IDLE);

        delay(50);

        // send completion notification
        this->sendJobCompletionNotification(this->wcsIn.actionEnum, "01");
#else
        slaveHandler.createRetrievalSteps(this->wcsIn.instructions);
        slaveHandler.beginNextStep();
#endif

        break;
    }
    case STOREBIN:
    {
        info("REC WCS::STORAGE");
#ifdef TCP_STRESS_TEST
        //set status active
        status.setState(STORING);

        delay(1000);

        // set status idle
        status.setState(IDLE);

        delay(50);

        // send completion notification
        this->sendJobCompletionNotification(this->wcsIn.actionEnum, "01");
#else
        slaveHandler.createStorageSteps(this->wcsIn.instructions);
        slaveHandler.beginNextStep();
#endif

        break;
    }
    case MOVE:
    {
        info("REC WCS::MOVE");
#ifdef TCP_STRESS_TEST
        //set status active
        status.setState(MOVING);

        delay(1000);

        // set status idle
        status.setState(IDLE);

        delay(50);

        // send completion notification
        this->sendJobCompletionNotification(this->wcsIn.actionEnum, "01");
#else
        slaveHandler.createMovementSteps(this->wcsIn.instructions);
        slaveHandler.beginNextStep();
#endif
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
    case SET:
    {
        info("REC WCS::SET");
        // determine what to set, remaining instructions are set instructions
        char setType[DEFAULT_SET_TYPE_ENUM_LENGTH + 1];
        strcut(setType, this->wcsIn.instructions, 0, DEFAULT_SET_TYPE_ENUM_LENGTH);
        ENUM_MANUAL_SET_TYPES setTypeInt = (ENUM_MANUAL_SET_TYPES)atoi(setType);
        switch (setTypeInt)
        {
        case SET_DEFAULT:
        {
            // sets id, level and status to default values
            status.setDefault();
            break;
        }
        case SET_ID:
        {
            if (strlen(this->wcsIn.instructions) > 0)
                status.setId(this->wcsIn.instructions);
            else
                status.setId(DEFAULT_ID);
            info("Id updated");
            break;
        }
        case SET_LEVEL:
        {
            if (strlen(this->wcsIn.instructions) > 0)
            {
                status.setLevel(this->wcsIn.instructions);
            }
            else
                status.setLevel(DEFAULT_LEVEL);
            info("Level updated");
            break;
        }
        case SET_STATE:
        {
            if (strlen(this->wcsIn.instructions) > 0)
            {
                SHUTTLE_STATE manualState = (SHUTTLE_STATE)atoi(this->wcsIn.instructions);
                status.setState(manualState);
            }
            else
                status.setState(SHUTTLE_STATE::IDLE);
            info("State updated");
            break;
        }
        case SET_ECHO:
        {
            resetEcho();
            break;
        }
        default:
            break;
        }
        // save changes to sd
        status.saveStatus();
        break;
    }
    // Handle echos received from wcs
    case ECHO:
    {
        char *awaitingEcho = this->getEcho();
        char echoReply[DEFAULT_CHAR_ARRAY_SIZE];
        if (strlen(awaitingEcho) <= 0)
        {
            // for whatever reason, echo to check is missing
            sprintf(echoReply, "Echo record is empty against %s", this->wcsIn.instructions);
            info(echoReply);
            this->resetEchoRetries();
            this->clearEchoTimeout();
            this->confirmEcho();
        }
        else if (strncmp(awaitingEcho, this->wcsIn.instructions, strlen(this->wcsIn.instructions)) == 0)
        {
            sprintf(echoReply, "%s echo received", this->wcsIn.instructions);
            info(echoReply);
            this->confirmEcho();
            this->resetEchoRetries();
            this->clearEchoTimeout();

            // check if expecting more echos
            // set timeout if more echos are unanswered
            if (this->echoBufferReadPointer != this->echoBufferWritePointer)
                this->setEchoTimeout();
        }
        else
        {
            sprintf(echoReply, "%s - %s echo not equal", awaitingEcho, this->wcsIn.instructions);
            info(echoReply);
        }
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

void WcsHandler::handle()
{
    // check for header and footer index
    int headerIndex = findIndex(this->readBuffer, STX[0]);
    int footerIndex = findIndex(this->readBuffer, ETX[0]);

    while (headerIndex >= 0 && footerIndex > headerIndex)
    {
        // extract input
        char temp[DEFAULT_CHAR_ARRAY_SIZE];
        strcut(temp, this->readBuffer, headerIndex, footerIndex - headerIndex + 1);

        // clean out STX and ETX chars
        char cleanInput[DEFAULT_CHAR_ARRAY_SIZE];
        strcut(cleanInput, temp, 1, strlen(temp) - 2);

        // interpret input
        bool interpretRes = this->interpret(cleanInput);
        if (!interpretRes)
            return;

        // echo back what was received
        if (this->wcsIn.action != ECHO && this->wcsIn.action != PING)
            this->sendEcho(cleanInput);

        // perform
        this->perform();

        // check for more inputs
        if (strlen(this->readBuffer) > 0)
        {
            headerIndex = findIndex(this->readBuffer, STX[0]);
            footerIndex = findIndex(this->readBuffer, ETX[0]);
        }
        else
        {
            headerIndex = -1;
            footerIndex = -1;
        }
    }
}

bool WcsHandler::send(char *str, bool shouldLog, bool awaitEcho)
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

    static char wcsSendLog[DEFAULT_CHAR_ARRAY_SIZE * 4];

    if (!res)
        sprintf(wcsSendLog, "Failed to send to server: %s", writeString);
    else
        sprintf(wcsSendLog, "Sent to server: %s", writeString);

    if (shouldLog)
        logSd(wcsSendLog);
    writeString[0] = '\0';

    if (awaitEcho)
    {
        char echoLog[DEFAULT_CHAR_ARRAY_SIZE];
        sprintf(echoLog, "%s expecting echo", str);
        info(echoLog);
        // save echo to echo buffer
        this->addEcho(str);
        this->setEchoTimeout();
    }

    delay(200); // forced artificial delay to await any echos

    return res;
}

bool WcsHandler::send(bool shouldLog, bool awaitEcho)
{
    // WcsHandler::send overload.
    // compiles wcsOut into a cstring to send
    char wcsOutString[DEFAULT_CHAR_ARRAY_SIZE];

    strcpy(wcsOutString, this->wcsOut.id);
    strcat(wcsOutString, this->wcsOut.actionEnum);
    if (strlen(this->wcsOut.instructions) > 0)
        strcat(wcsOutString, this->wcsOut.instructions);
    bool res = this->send(wcsOutString, shouldLog, awaitEcho);

    // empty out wcsOut
    this->wcsOut.actionEnum[0] = '\0';
    this->wcsOut.instructions[0] = '\0';

    return res;
};

bool WcsHandler::send(bool shouldLog)
{
    return this->send(shouldLog, true);
}

bool WcsHandler::send()
{
    return this->send(true, true);
}

bool WcsHandler::sendEcho(char *echo)
{
    char wcsOutString[DEFAULT_CHAR_ARRAY_SIZE];
    strcpy(wcsOutString, this->wcsOut.id);
    char echoActionString[3];
    GET_TWO_DIGIT_STRING(echoActionString, ECHO);
    strcat(wcsOutString, echoActionString);
    strcat(wcsOutString, echo);
    return this->send(wcsOutString, false, false);
}

void WcsHandler::pullCurrentStatus()
{
    // retrieves status information
    strcpy(this->wcsOut.id, status.getId());
    if (!status.isIdDefault())
    {
        strcpy(this->wcsOut.instructions, status.getLevel());
        char stateStr[3];
        GET_TWO_DIGIT_STRING(stateStr, status.getState());
        strcat(this->wcsOut.instructions, stateStr);
    }
};

void WcsHandler::updateLastPing()
{
    this->lastPingMillis = millis();
};

bool WcsHandler::isPingAlive()
{
    if (millis() - this->lastPingMillis > PING_DROPPED_DURATION)
        return false;
    return true;
};

void WcsHandler::setEchoTimeout()
{
    if (this->echoTimeoutMillis == 0)
        this->echoTimeoutMillis = millis() + ECHO_TIMEOUT_DURATION;
};

bool WcsHandler::isEchoTimeout()
{
    if (this->echoTimeoutMillis <= 0)
        return false;

    if (millis() >= this->echoTimeoutMillis)
        return true;
    return false;
};

bool WcsHandler::clearEchoTimeout()
{
    this->echoTimeoutMillis = 0;
    return true;
};

bool WcsHandler::incEchoRetries()
{
    this->echoRetries++;
    return true;
};

bool WcsHandler::resetEchoRetries()
{
    this->echoRetries = 0;
    return true;
};

bool WcsHandler::addEcho(char *str)
{
    // if write pointer is lapping, do no overwrite un read read pointer
    if (this->echoBufferPointerLapping && this->echoBufferWritePointer == this->echoBufferReadPointer)
        return false;

    // write to buffer
    strcpy(this->echoBuffer[this->echoBufferWritePointer], str);
    // move write pointer
    this->echoBufferWritePointer = (this->echoBufferWritePointer + 1) % ECHO_BUFFER_SIZE;
    if (this->echoBufferWritePointer == 0 && this->echoBufferWritePointer < this->echoBufferReadPointer)
        this->echoBufferPointerLapping = true;
    return true;
};

char *WcsHandler::getEcho()
{
    static char output[DEFAULT_CHAR_ARRAY_SIZE];
    strcpy(output, this->echoBuffer[this->echoBufferReadPointer]);
    return output;
};

bool WcsHandler::confirmEcho()
{
    // read pointer should not overrun write pointer.
    if (!this->echoBufferPointerLapping && this->echoBufferReadPointer == this->echoBufferWritePointer)
        return false;

    // move read pointer
    this->echoBufferReadPointer = (this->echoBufferReadPointer + 1) % ECHO_BUFFER_SIZE;

    // check if read pointer has caught up with write pointer
    if (this->echoBufferReadPointer == 0 && this->echoBufferPointerLapping)
        this->echoBufferPointerLapping = false;

    return true;
};

bool WcsHandler::resetEcho()
{
    this->echoBufferWritePointer = 0;
    this->echoBufferReadPointer = 0;
    for (int i = 0; i < ECHO_BUFFER_SIZE; i++)
    {
        this->echoBuffer[i][0] = '\0';
    }
    this->clearEchoTimeout();
    this->resetEchoRetries();
};

void WcsHandler::saveAndResetChip()
{
    // write unreplied echo to chip EEPROM
    if (
        (this->echoBufferPointerLapping && this->echoBufferReadPointer > this->echoBufferWritePointer) 
        || (!this->echoBufferPointerLapping && this->echoBufferReadPointer < this->echoBufferWritePointer))
        writeEEPROMMsg(this->echoBuffer[this->echoBufferReadPointer]);
    // reset the chip
    resetChip();
};

// --------------------------
// Wcs Public Methods
// --------------------------
WcsHandler::WcsHandler()
{
    this->echoBufferWritePointer = 0;
    this->echoBufferReadPointer = 0;
    this->echoBufferPointerLapping = false;
    this->echoTimeoutMillis = 0;
    this->echoRetries = 0;

    this->lastPingMillis = millis();
};

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
        info("Failed to connect to wifi.");
        this->saveAndResetChip();
    }
    info("Wifi connected");

    // wcs connection
    info("Connecting to WCS Server");
    bool tcpConnectionRes = ConnectTcpServer();
    if (!tcpConnectionRes)
    {
        logSd("Failed to connect to server.");
        this->saveAndResetChip();
    }
    info("server connected");
    // retrieve existing shuttle id
    this->pullCurrentStatus();
    // login to server
    char loginEnumString[3];
    GET_TWO_DIGIT_STRING(loginEnumString, LOGIN);
    strcpy(this->wcsOut.actionEnum, loginEnumString);
    this->send(true, false);
}

void WcsHandler::run(void)
{
    if (!this->isPingAlive())
    {
        // no more pings are received from server. restart chip to attempt reconnection
        logSd("No pings from server. Restarting chip.");
        this->saveAndResetChip();
    }
    else
    {
        // check for echo buffer timeout
        if (this->isEchoTimeout())
        {
            // inc retries count
            this->incEchoRetries();
            // clear timeout
            this->clearEchoTimeout();
            info("increased echo retries count");

            if (this->echoRetries <= 3)
            {
                info("Resending unechoed message");
                // resend unreplied message
                this->send(this->getEcho(), true, false);
            }
            else
            {
                logSd("Failed 3 send retries. Resetting chip");
                this->saveAndResetChip();
            }
        }

        // check for input from wcs
        if (TcpRead(this->readBuffer))
            this->handle();
    }
}

bool WcsHandler::sendJobCompletionNotification(const char *actionEnum, const char *inst)
{
    char completionString[DEFAULT_CHAR_ARRAY_SIZE * 2];
    sprintf(completionString, "Notify steps completion for action %s, inst %s", actionEnum, inst);
    info(completionString);
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