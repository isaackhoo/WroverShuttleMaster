#include "Helper.h"
#include "LCD.h"
#include "SD.h"

// Append char
void appendChar(char *s, char c)
{
    int len = strlen(s);
    s[len] = c;
    s[len + 1] = '\0';
};

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

// Convert to c string
char *toCString(int i)
{
    static char intCStr[DEFAULT_CHAR_ARRAY_SIZE];
    sprintf(intCStr, "%d", i);
    return intCStr;
};

char *toCString(long li)
{
    static char longCStr[DEFAULT_CHAR_ARRAY_SIZE];
    sprintf(longCStr, "%li", li);
    return longCStr;
};

char *toCString(double d)
{
    static char doubleCStr[DEFAULT_CHAR_ARRAY_SIZE];
    sprintf(doubleCStr, "%lf", d);
    return doubleCStr;
};

char *toCString(float f)
{
    static char floatCStr[DEFAULT_CHAR_ARRAY_SIZE];
    sprintf(floatCStr, "%f", f);
    return floatCStr;
};

// Char array substring
bool charArrSubString(char *source, char *output, int from, int length)
{
    // extracts a portion of the source string to output
    int sourceLength = strlen(source);

    if (from < 0 || length < 0)
        return false;
    if (from > sourceLength)
        return false;

    int i = 0, j;
    if (length > 0 && from + length - 1 < sourceLength)
    {
        for (i = 0, j = from; j <= from + length - 1; j++, i++)
        {
            output[i] = source[j];
        }
        output[i] = '\0';
    }
    else if (length == 0 && from < sourceLength)
    {
        for (i = 0, j = from; j < sourceLength; j++, i++)
        {
            output[i] = source[j];
        }
        output[i] = '\0';
    }
    else
    {
        output[0] = '\0'; // empty string
    }
    return true;
}

// Cut string
bool cutStr(char *source, char *output, int from, int length)
{
    // takes a source string and replaces it with the remainders after the cut.
    // cut out portion is return via output.

    int sourceLength = strlen(source);

    if (from < 0 || length < 0)
        return false;
    if (from > sourceLength)
        return false;

    // extract cut portion to output
    int i = 0, j;
    for (j = from; j < from + length; j++, i++)
    {
        output[i] = source[j];
    }
    output[i] = '\0'; // terminates output

    // replace source with remainder of string
    // removes everything else before 'from' as well
    if (from + length < sourceLength)
    {
        for (i = 0, j = from + length; j < sourceLength; j++, i++)
        {
            source[i] = source[j];
        }
        source[i] = '\0';
    }
    else
    {
        source[0] = '\0'; // empty string
    }
    return true;
};

// Search c string
int findIndex(char *str, char c)
{
    const char *ptr = strchr(str, c);
    if (ptr)
        return ptr - str;
    return -1;
}

int reverseFindIndex(char *str, char c)
{
    const char *ptr = strrchr(str, c);
    if (ptr)
        return ptr - str;
    return -1;
}

// Serial / LCD toggle
void initSerial()
{
    Serial.begin(DEFAULT_SERIAL_BAUD_RATE);
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
    if (strlen(str) <= 0)
        return;
    Serial.println("sending to lcd");
    Serial.println(str);
    LcdScrollText(str);
    Serial.println("done printing");
};

void outToLcdSd(char *str)
{
    LcdScrollText(str);
    logToSd(str);
};

// String to char array
// void stringToCharArr(char *c, String s)
// {
//     int stringLength = s.length();
//     for (int i = 0; i < stringLength; i++)
//         c[i] = s[i];
//     c[stringLength] = '\0';
// }