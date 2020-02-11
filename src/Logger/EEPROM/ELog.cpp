#include "ELog.h"
#include "../Logger.h"

void beginEEPROM()
{
    if(EEPROM.begin(EEPROM_SIZE))
        info("EEPROM initialized");
    else
        info("Failed to initialize EEPROM");
}

void writeEEPROM(char *input) {
    for (int i = 0; i < strlen(input); i++) {
        EEPROM.write(i, input[i]);
    }
    EEPROM.commit();
};

void clearEEPROM() {
    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(i, '\0');
    }
    EEPROM.commit();
};

char *readEEPROM() {
    static char output[EEPROM_SIZE + 1];
    for(int i = 0; i < EEPROM_SIZE; i++) {
        output[i] = (char)EEPROM.read(i);
    }
    output[EEPROM_SIZE + 1] = '\0';
    return output;
};