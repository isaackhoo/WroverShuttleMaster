#ifndef SD_H
#define SD_H

// sd public methods
extern bool SdInit();
extern bool logToSd(const char*);
extern bool logTimestampToSd();

#endif