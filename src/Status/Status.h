#pragma once

#ifndef STATUS_H
#define STATUS_H

#include "../Helper/Helper.h"

// -------------------------
// Status Private Variables
// -------------------------
const int DEFAULT_ID_LENGTH = 4;
static const char *DEFAULT_ID = "-1-1";
static const char *DEFAULT_LEVEL = "02";
static const int DEFAUULT_POS = 60000; // TODO: change to actual rear position value

static const char statusLogDelimiter[2] = ",";

// -------------------------
// Status Public Variables
// -------------------------
class Status;
extern Status status;
typedef enum
{
  IDLE = 0,
  STORING,
  RETRIEVING,
  MOVING,
  SHUTTLE_ERROR,
  AWAITING,
  Num_Of_Shuttle_States
} SHUTTLE_STATE;
static const char SHUTTLE_STATE_STRING[][15] = {"IDLE", "STORING", "RETRIEVING", "MOVING", "ERROR", "AWAITING"};

// -------------------------
// Status Public Methods
// -------------------------
class Status
{
private:
  char id[5];
  char actionEnum[3];
  char instructions[DEFAULT_CHAR_ARRAY_SIZE];
  char currentLevel[3];
  int currentPos;
  SHUTTLE_STATE state;

  // hardware status
  bool isCarryingBin;
  bool isFingerExtended;

public:
  Status();
  ~Status();
  bool setId(const char *);
  bool setActionEnum(const char *);
  bool setInstructions(const char *);
  bool setLevel(const char *);
  bool setPos(const int);
  bool setState(SHUTTLE_STATE);
  bool setActiveState();
  bool setIsCarryingBin(bool);
  bool setIsFingerExtended(bool);
  bool setWcsInputs(const char *, const char *);
  void rehydrateStatus(char *);

  char *getId();
  char *getActionEnum();
  char *getInstructions();
  char *getLevel();
  int getPos();
  SHUTTLE_STATE getState();
  bool getIsCarryingBin();
  bool getIsFingerExtended();
  bool isIdDefault();

  void saveStatus();
};

#endif