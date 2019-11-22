#ifndef HELPER_H
#define HELPER_H

#include <string.h>

// Array size
#define GET_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

// Char array size
#define DEFAULT_CHAR_ARRAY_SIZE 128

// Convert int to 2 char digit string, i.e. int 1 -> "01"
extern void GET_TWO_DIGIT_STRING(char *, int);
extern char *GET_TWO_DIGIT_STRING(int);

// Serial baud
#define DEFAULT_SERIAL_BAUD_RATE 115200

// Serial / LCD toggle

// #define DEV_ENV // comment out during production

#ifdef DEV_ENV
#define initPrinter() initSerial()
#define info(x) serialPrintln(x)
#define log(x) serialPrintln(x)
#else
#define initPrinter() initLcdSd()
#define info(x) outToLcd(x)
#define log(x) outToLcdSd(x)
#endif

#ifdef DEV_ENV
extern void initSerial();
extern void serialPrintln(char*);
#else
extern void initLcdSd();
extern void outToLcd(char *);
extern void outToLcdSd(char *);
#endif



#endif