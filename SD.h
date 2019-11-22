#ifndef SD_H
#define SD_H

#include "FS.h"

// sd methods
bool createDir(fs::FS &, const char *);
bool removeDir(fs::FS &, const char *);
String readFile(fs::FS &, const char *);
bool writeFile(fs::FS &, const char *, const char *);
bool appendFile(fs::FS &, const char *, const char *);
bool renameFile(fs::FS &, const char *, const char *);
bool deleteFile(fs::FS &, const char *);

// sd public methods
extern bool SdInit();
extern bool logToSd(const char*);
extern bool logTimestampToSd();

#endif