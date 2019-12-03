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
bool Status::setId(const char *id)
{
    if (strlen(id) != DEFAULT_ID_LENGTH)
        return false;
    // strcpy_s(this->id, sizeof this->id, id);
    strcpy(this->id, id);

    return true;
};

bool Status::setActionEnum(const char *action)
{
    // strcpy_s(this->actionEnum, sizeof this->actionEnum, action);
    strcpy(this->actionEnum, action);
};

bool Status::setInstructions(const char *inst)
{
    this->instructions[0] = '\0';
    // strcpy_s(this->instructions, sizeof this->instructions, inst);
    strcpy(this->instructions, inst);
};

bool Status::setLevel(const char *lvl)
{
    strcpy(this->currentLevel, lvl);
};

bool Status::setPos(const int pos)
{
    this->currentPos = pos;
};

bool Status::setState(SHUTTLE_STATE currentState)
{
    if (currentState < 0 || currentState > Num_Of_Shuttle_States)
        return false;
    if (this->getState() != currentState)
    {
        this->state = currentState;
        char statusChange[DEFAULT_CHAR_ARRAY_SIZE];
        sprintf(statusChange, "Status updated to %s", SHUTTLE_STATE_STRING[this->state]);
        logSd(statusChange);
        wcsHandler.updateStateChange();
    }
};

bool Status::setActiveState()
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

bool Status::setIsCarryingBin(bool isCarryingBin)
{
    this->isCarryingBin = isCarryingBin;
};

bool Status::setIsFingerExtended(bool isFingerExtended)
{
    this->isFingerExtended = isFingerExtended;
};

bool Status::setWcsInputs(const char *actionEnum, const char *inst)
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
            logSd("rehydrated Id");
            break;
        }
        case 1:
        {
            // current level
            this->setLevel(token);
            logSd("rehydrated level");
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
    char statusString[DEFAULT_CHAR_ARRAY_SIZE];
    strcpy(statusString, this->getId());
    strcat(statusString, statusLogDelimiter);
    strcat(statusString, getLevel());

    logStatus(statusString);

    char logStatusChangeInfo[DEFAULT_CHAR_ARRAY_SIZE];
    sprintf(logStatusChangeInfo, "saved to status: %s", statusString);
    logSd(logStatusChangeInfo);
};