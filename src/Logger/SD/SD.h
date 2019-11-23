#ifndef SD_H
#define SD_H

// sd public variables
extern const int timestampInterval;

// sd public methods
extern bool SdInit();
extern bool logToSd(const char*);
extern void logTimestampCallback(int, int, int);

#endif