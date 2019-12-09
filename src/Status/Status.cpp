#include <string.h>
#include <stdio.h>
#include "Status.h"
#include "../WcsHandler/WcsHandler.h"
#include "../Helper/Helper.h"
#include "../Logger/Logger.h"
#include "../Logger/SD/SD.h"

// -------------------------
// Status Public Variables
// -------------------------
Status status;

// -------------------------
// Status Private Methods
// -------------------------

// -------------------------
// Status Public Methods
// -------------------------
Status::Status()
{
    setId(DEFAULT_ID);
    setLevel(DEFAULT_LEVEL);
    setPos(DEFAUULT_POS);
}

Status::~Status(){};

// SETTERS
void Status::setId(const char *id)
{
    if (strlen(id) != DEFAULT_ID_LENGTH)
        return;

    strcpy(this->id, id);
};

void Status::setActionEnum(const char *action)
{
    // strcpy_s(this->actionEnum, sizeof this->actionEnum, action);
    strcpy(this->actionEnum, action);
};

void Status::setInstructions(const char *inst)
{
    this->instructions[0] = '\0';
    // strcpy_s(this->instructions, sizeof this->instructions, inst);
    strcpy(this->instructions, inst);
};

void Status::setLevel(const char *lvl)
{
    strcpy(this->currentLevel, lvl);
};

void Status::setPos(const int pos)
{
    this->currentPos = pos;
};

void Status::setState(SHUTTLE_STATE currentState, bool logAndSend)
{
    if (currentState < 0 || currentState > Num_Of_Shuttle_States)
        return;
    if (this->getState() != currentState)
    {
        this->state = currentState;
        char statusChange[DEFAULT_CHAR_ARRAY_SIZE];
        sprintf(statusChange, "Status updated to %s", SHUTTLE_STATE_STRING[this->state]);
        if (logAndSend)
        {
            logSd(statusChange);
            this->saveStatus();
            wcsHandler.updateStateChange();
        }
    }
};

void Status::setState(SHUTTLE_STATE currentState)
{
    this->setState(currentState, true);
};

void Status::setActiveState()
{
    // sets state based on current action enum
    SHUTTLE_STATE activeState = SHUTTLE_ERROR;
    switch ((ENUM_WCS_ACTIONS)atoi(this->actionEnum))
    {
    case RETRIEVEBIN:
    {
        activeState = RETRIEVING;
        break;
    }
    case STOREBIN:
    {
        activeState = STORING;
        break;
    }
    case MOVE:
    {
        activeState = MOVING;
        break;
    }
    default:
        break;
    }
    this->setState(activeState);
};

void Status::setIsCarryingBin(bool isCarryingBin)
{
    this->isCarryingBin = isCarryingBin;
};

void Status::setIsFingerExtended(bool isFingerExtended)
{
    this->isFingerExtended = isFingerExtended;
};

void Status::setWcsInputs(const char *actionEnum, const char *inst)
{
    this->setActionEnum(actionEnum);
    this->setInstructions(inst);
};

void Status::rehydrateStatus(char *hydrator)
{
    if (strlen(hydrator) <= 0)
        return;

    int extrationPos = 0;
    char *token = strtok(hydrator, statusLogDelimiter);
    while (token != NULL)
    {
        switch (extrationPos)
        {
        case 0:
        {
            // shuttle id
            this->setId(token);
            info("rehydrated Id");
            break;
        }
        case 1:
        {
            // current level
            this->setLevel(token);
            info("rehydrated level");
            break;
        }
        case 2:
        {
            // state
            this->setState((SHUTTLE_STATE)atoi(token), false);
            info("rehydrated state");
            break;
        }
        default:
            break;
        }
        extrationPos++;
        token = strtok(NULL, statusLogDelimiter);
    }
};

// GETTERS
char *Status::getId() { return this->id; };
char *Status::getActionEnum() { return this->actionEnum; };
char *Status::getInstructions() { return this->instructions; };
char *Status::getLevel() { return this->currentLevel; };
int Status::getPos() { return this->currentPos; };
SHUTTLE_STATE Status::getState() { return this->state; };
bool Status::getIsCarryingBin() { return this->isCarryingBin; };
bool Status::getIsFingerExtended() { return this->isFingerExtended; };
bool Status::isIdDefault() { return strcmp(this->getId(), DEFAULT_ID) == 0 ? true : false; };
void Status::saveStatus()
{
    static char statusString[DEFAULT_CHAR_ARRAY_SIZE];
    strcpy(statusString, this->getId());
    strcat(statusString, statusLogDelimiter);
    strcat(statusString, getLevel());
    strcat(statusString, statusLogDelimiter);
    char stateString[3];
    GET_TWO_DIGIT_STRING(stateString, this->getState());
    strcat(statusString, stateString);

    logStatus(statusString);

    static char logStatusChangeInfo[DEFAULT_CHAR_ARRAY_SIZE];
    sprintf(logStatusChangeInfo, "saved to status: %s", statusString);
    logSd(logStatusChangeInfo);
};