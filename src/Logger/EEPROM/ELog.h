#include "EEPROM.h"

#define EEPROM_SIZE 64

extern void beginEEPROM();
extern void writeEEPROM(char *);
extern void clearEEPROM();
extern char *readEEPROM();