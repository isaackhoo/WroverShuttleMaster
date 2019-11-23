#include <HardwareSerial.h>
#include <Automaton.h>
#include "Logger.h"
#include "./LCD/LCD.h"
#include "../Helper/Helper.h"
#include "./SD/SD.h"

Atm_timer timestampInvervalHandler;

// Serial / LCD toggle
void initSerial()
{
    Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
};

void initLcdSd()
{
    // initialises lcd and sd card reader
    initSerial();
    LcdInit();
    SdInit();
    timestampInvervalHandler.begin(timestampInterval)
        .repeat(ATM_COUNTER_OFF) // repeat infefinitely
        .onTimer(logTimestampCallback)
        .start();
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
