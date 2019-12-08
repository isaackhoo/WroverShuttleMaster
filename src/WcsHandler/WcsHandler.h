#pragma once

#ifndef WCSHANDLER_H
#define WCSHANDLER_H

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
    char instructions[128];
} WcsFormat;

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
    char tcpReadBuff[1024];
    WcsFormat wcsIn;
    WcsFormat wcsOut;
    bool interpret(char *);
    void perform();

  public:
    void init();
    bool sendToServer(const char *);
    bool sendToServer();
    void run();
};

#endif