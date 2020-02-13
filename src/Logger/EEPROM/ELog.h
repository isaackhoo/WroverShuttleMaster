#include "EEPROM.h"

#define EEPROM_MSG_SIZE 64
#define EEPROM_MSG_START_POS 0

extern void beginEEPROM();

// msg
extern void writeEEPROMMsg(char *);
extern void clearEEPROMMsg();
extern char *readEEPROMMsg();