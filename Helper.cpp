#include "Helper.h"
#include "LCD.h"
#include "SD.h"

// Convert int to 2 char digit string, i.e. int 1 -> "01"
void GET_TWO_DIGIT_STRING(char *output, int x)
{
    sprintf(output, "%02d", x);
};

char *GET_TWO_DIGIT_STRING(int x)
{
    static char output[4];
    GET_TWO_DIGIT_STRING(output, x);
    return output;
};

// Prod printer
void initSerial()
{
    Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
};

void serialPrintln(char * str){
    Serial.println(str);
};

void initLcdSd()
{
    // initialises lcd and sd card reader
    initSerial();
    SdInit();
    LcdInit();
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