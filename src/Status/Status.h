#pragma once

#ifndef STATUS_H
#define STATUS_H

#include "../Helper/Helper.h"

// -------------------------
// Status Private Variables
// -------------------------
const int DEFAULT_ID_LENGTH = 4;
static const char *DEFAULT_ID = "-1-1";
static const int DEFAULT_LEVEL = 2;
static const int DEFAUULT_POS = 60000; // TODO: change to actual rear position value
static const bool DEFAULT_ISACTIVE = false;

// -------------------------
// Status Public Variables
// -------------------------
class Status;
extern Status status;

// -------------------------
// Status Public Methods
// -------------------------
class Status
{
  private:
    char id[5];
    char actionEnum[3];
    char instructions[DEFAULT_CHAR_ARRAY_SIZE];
    int currentLevel;
    int currentPos;
    bool isActive;

  public:
    Status();
    ~Status();
    bool setId(const char *);
    bool setActionEnum(const char *);
    bool setInstructions(const char *);
    bool setLevel(const int);
    bool setPos(const int);
    bool setIsActive(const bool);

    char *getId();
    char *getActionEnum();
    char *getInstructions();
    int getLevel();
    int getPos();
    bool getIsActive();
};

#endif