#include "Status.h"
#include <string.h>

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
    setIsActive(DEFAULT_ISACTIVE);
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

bool Status::setIsActive(const bool isActive)
{
    this->isActive = isActive;
};

// GETTERS
char *Status::getId() { return this->id; };
char *Status::getActionEnum() { return this->actionEnum; };
char *Status::getInstructions() { return this->instructions; };
int Status::getLevel() { return this->currentLevel; };
int Status::getPos() { return this->currentPos; };
bool Status::getIsActive() { return this->isActive; };