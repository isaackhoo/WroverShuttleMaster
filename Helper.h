#ifndef HELPER_H
#define HELPER_H

// Append char
extern void appendChar(char *, char);

// Array size
#define GET_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

// Char array size
#define DEFAULT_CHAR_ARRAY_SIZE 128

// Char array substring
extern bool charArrSubString(char *, char *, int, int);

// Convert int to 2 char digit string, i.e. int 1 -> "01"
extern void GET_TWO_DIGIT_STRING(char *, int);
extern char *GET_TWO_DIGIT_STRING(int);

// Convert to c string
extern char* toCString(int);
extern char* toCString(long);
extern char* toCString(double);
extern char* toCString(float);


// Cut str
extern bool cutStr(char *, char *, int, int);

// Search c string
extern int findIndex(char*, char);
extern int reverseFindIndex(char*, char);

// Serial baud
#define DEFAULT_SERIAL_BAUD_RATE 115200

// Serial / LCD toggle
//#define DEV_ENV // comment out during production
#ifdef DEV_ENV
#define initPrinter() initSerial()
#define info(x) Serial.println(x)
#define log(x) Serial.println(x)
#else
#define initPrinter() initLcdSd()
#define info(x) outToLcd(x)
#define log(x) outToLcdSd(x)
#endif

#ifdef DEV_ENV
extern void initSerial();
#else
extern void initLcdSd();
extern void outToLcd(char*);
extern void outToLcdSd(char*);
#endif

// String to char array
// extern void stringToCharArr(char*, String);

#endif