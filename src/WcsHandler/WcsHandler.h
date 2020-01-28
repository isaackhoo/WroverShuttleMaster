#pragma once

#ifndef WCSHANDLER_H
#define WCSHANDLER_H

#include "../Helper/Helper.h"

// #define TCP_STRESS_TEST

// --------------------------
// Wcs Private Variables
// --------------------------
typedef enum
{
  NONE = 0,
  LOGIN,       // 1
  LOGOUT,      // 2
  PING,        // 3
  RETRIEVEBIN, // 4
  STOREBIN,    // 5
  MOVE,        // 6
  BATTERY,     // 7
  STATE,       // 8
  LEVEL,       // 9
  SET,         // 10
  ERROR,       // 11
  Num_Of_WCS_Action_Enums
} ENUM_WCS_ACTIONS;
const int DEFAULT_ACTION_ENUM_LENGTH = 2;
typedef enum
{
  SET_DEFAULT = 0,
  SET_ID,       // 01
  SET_LEVEL,    // 02
  SET_STATE,   // 03
  Num_Of_Manual_Set_Types_Enums
} ENUM_MANUAL_SET_TYPES;
const int DEFAULT_SET_TYPE_ENUM_LENGTH = 2;
static const char *STX = "\x02";
static const char *ETX = "\x03";
typedef struct
{
  char id[5];
  char actionEnum[3];
  int action;
  char instructions[DEFAULT_CHAR_ARRAY_SIZE];
} WcsFormat;
static const long PING_DROPPED_DURATION = 1000 * (30 + 10); // 30s ping duration, +10s buffer to determine ping has dropped

// --------------------------
// Wcs Public Variables
// --------------------------
class WcsHandler;
extern WcsHandler wcsHandler;

// --------------------------
// Wcs Public Methods
// --------------------------
class WcsHandler
{
private:
  char readBuffer[DEFAULT_CHAR_ARRAY_SIZE * 2];
  WcsFormat wcsIn;
  WcsFormat wcsOut;
  unsigned long lastPingMillis;
  bool interpret(char *);
  void perform();
  void handle();
  bool send(char *, bool);
  bool send(bool);
  bool send();
  void pullCurrentStatus();
  void updateLastPing();
  bool isPingAlive();

public:
  WcsHandler();
  void init(void);
  void run(void);
  bool sendJobCompletionNotification(const char *, const char *);
  bool updateStateChange();
};

#endif