#include <string.h>
#include <stdio.h>
#include "helper.h"

// Cut c string
// negative length removes everything up to the end
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

// Convert int to 2 char digit string, i.e. int 1 -> "01"
void GET_TWO_DIGIT_STRING(char *output, int x)
{
    sprintf(output, "%02d", x);
};

// Search c string
int findIndex(char *str, char c)
{
    const char *ptr = strchr(str, c);
    if (ptr)
        return ptr - str;
    return -1;
}
