#include <HardwareSerial.h>
#include <Automaton.h>
#include "Logger.h"
#include "./LCD/LCD.h"
#include "../Helper/Helper.h"
#include "./SD/SD.h"

Atm_timer timestampInvervalHandler;
Atm_timer batchLogIntervalHandler;

// Serial / LCD toggle
void initSerial()
{
    Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
};

void initLcdSd()
{
    // initialises lcd and sd card reader
    LcdInit();
    bool sdRes = SdInit();
    if (sdRes)
    {
        timestampInvervalHandler.begin(timestampInterval)
            .repeat(ATM_COUNTER_OFF) // repeat infefinitely
            .onTimer(logTimestampCallback)
            .start();
        batchLogIntervalHandler.begin(batchLogInterval)
            .repeat(ATM_COUNTER_OFF)
            .onTimer(batchLogCallback)
            .start();
    }
    else
    {
        info("Sd failed to initialize");
    }
};

void outToLcd(char *str)
{
    LcdScrollText(str);
};

void outToLcdSd(char *str)
{
    LcdScrollText(str);
    addToSdPending(str);
    // logToSd(str);
};
