#pragma once

#ifndef WCSHANDLER_H
#define WCSHANDLER_H

#include "../Helper/Helper.h"

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
  ERROR,
  Num_Of_WCS_Action_Enums
} ENUM_WCS_ACTIONS;
const int DEFAULT_ACTION_ENUM_LENGTH = 2;
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
  int hasCompleteInstructions();
  int read();
  bool interpret(char *);
  void perform();
  void handle(int);
  bool send(char *, bool);
  bool send(bool);
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