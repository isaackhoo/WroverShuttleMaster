#include "Helper.h"
#include <string.h>
#include <stdio.h>
#include "../Logger/SD/SD.h"
#include "Esp.h"

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

// Convert to c string
void toCString(char* output, int i)
{
    sprintf(output, "%d", i);
};

void toCString(char* output, long li)
{
    sprintf(output, "%li", li);
};

void toCString(char* output, double d)
{
    sprintf(output, "%lf", d);
};

void toCString(char* output, float f)
{
    sprintf(output, "%f", f);
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
bool strcut(char *output, char *source, int from, int length, bool dropFront = true)
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
    int replaceStartIndex;
    if (dropFront)
        replaceStartIndex = 0;
    else
        replaceStartIndex = from;

    if (from + length < sourceLength)
    {
        for (i = replaceStartIndex, j = from + length; j < sourceLength; j++, i++)
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
bool strcut(char *output, char *source, int from, int length)
{
    strcut(output, source, from, length, true);
};

// reset chip
void resetChip()
{
    // logToSd();
    ESP.restart();
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