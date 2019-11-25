#include "Status.h"
#include <string.h>
#include "../WcsHandler/WcsHandler.h"

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
    strcpy(this->id, id);
    return true;
};

bool Status::setActionEnum(const char *action)
{
    strcpy(this->actionEnum, action);
};

bool Status::setInstructions(const char *inst)
{
    this->instructions[0] = '\0';
    strcpy(this->instructions, inst);
};

bool Status::setLevel(const int lvl)
{
    this->currentLevel = lvl;
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
        wcsHandler.updateStateChange();
    }
};

bool Status::setActiveState()
{
    // sets state based on current action enum
    SHUTTLE_STATE activeState = IDLE;
    switch ((ENUM_WCS_ACTIONS)atoi(actionEnum))
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

// GETTERS
char *Status::getId() { return this->id; };
char *Status::getActionEnum() { return this->actionEnum; };
char *Status::getInstructions() { return this->instructions; };
int Status::getLevel() { return this->currentLevel; };
int Status::getPos() { return this->currentPos; };
SHUTTLE_STATE Status::getState() { return this->state; };
bool Status::getIsCarryingBin() { return this->isCarryingBin; };
bool Status::getIsFingerExtended() { return this->isFingerExtended; };