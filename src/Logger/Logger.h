#ifndef LOGGER_H
#define LOGGER_H

// Serial / LCD toggle
// #define DEV_ENV // comment out during production
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

#endif