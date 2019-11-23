#include <HardwareSerial.h>
#include "Logger.h"
#include "./LCD/LCD.h"
#include "../Helper/Helper.h"
#include "./SD/SD.h"

// Serial / LCD toggle
void initSerial()
{
    Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
};

void initLcdSd()
{
    // initialises lcd and sd card reader
    LcdInit();
    SdInit();
};

void outToLcd(char *str)
{
    LcdScrollText(str);
};

void outToLcdSd(char *str)
{
    LcdScrollText(str);
    logToSd(str);
};
