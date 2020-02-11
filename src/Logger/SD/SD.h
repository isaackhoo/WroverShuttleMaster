#pragma once

#ifndef SD_H
#define SD_H

#include "../../Helper/Helper.h"

// sd private variables
static const int MAX_PENDING_LOGS_SIZE = 50;
static int pendingLogsCurPointer = 0;
static char pendingLogs[MAX_PENDING_LOGS_SIZE * DEFAULT_CHAR_ARRAY_SIZE];
static bool isInit;

// sd public variables
extern const int timestampInterval;
extern const int batchLogInterval;

// sd public methods
extern bool SdInit();
extern bool logToSd(char *);
extern bool logToSd();
extern void addToSdPending(const char *);
extern void logTimestampCallback(int, int, int);
extern void batchLogCallback(int, int, int);

extern void logStatus(char *);
extern char *readStatus();

extern void logEcho(char *);
extern char *readEcho();

#endif