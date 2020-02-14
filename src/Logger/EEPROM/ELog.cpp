#include "ELog.h"
#include "../Logger.h"

void beginEEPROM()
{
    if (EEPROM.begin(EEPROM_MSG_SIZE))
    {
        info("EEPROM initialized");
    }
    else
        info("Failed to initialize EEPROM");
}

// msg
void writeEEPROMMsg(char *input)
{
    // write to eeprom
    for (int i = EEPROM_MSG_START_POS; i < EEPROM_MSG_START_POS + strlen(input); i++)
    {
        EEPROM.write(i, input[i - EEPROM_MSG_START_POS]);
    }
    EEPROM.write(EEPROM_MSG_START_POS + strlen(input), '\0');
    EEPROM.commit();
};

void clearEEPROMMsg()
{
    for (int i = EEPROM_MSG_START_POS; i < EEPROM_MSG_START_POS + EEPROM_MSG_SIZE; i++)
    {
        EEPROM.write(i, '\0');
    }
    EEPROM.commit();
};

char *readEEPROMMsg()
{

    static char output[EEPROM_MSG_SIZE];
    for (int i = EEPROM_MSG_START_POS; i < EEPROM_MSG_START_POS + EEPROM_MSG_SIZE; i++)
    {
        output[i - EEPROM_MSG_START_POS] = (char)EEPROM.read(i);
    }
    output[EEPROM_MSG_SIZE] = '\0';
    return output;
};