#pragma once

#ifndef HELPER_H
#define HELPER_H

// Append char
extern void appendChar(char *, char);

// Array size
#define GET_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

// Char array size
#define DEFAULT_CHAR_ARRAY_SIZE 64

// Char array substring
extern bool charArrSubString(char *, char *, int, int);

// Convert int to 2 char digit string, i.e. int 1 -> "01"
extern void GET_TWO_DIGIT_STRING(char *, int);

// Convert to c string
extern void toCString(char*, int);
extern void toCString(char*, long);
extern void toCString(char*, double);
extern void toCString(char*, float);

// Cut str
extern bool cutStr(char *, char *, int, int);

// Search c string
extern int findIndex(char*, char);
extern int reverseFindIndex(char*, char);

// Serial baud
#define DEFAULT_SERIAL_BAUD_RATE 115200

#endif