#pragma once

#ifndef LOGGER_H
#define LOGGER_H

// Serial / LCD toggle
// comment out during production

// -----------------------------------
// DEV_NOTE:
// info() and log() can only accept
// c_string inputs. 
// Convert all other data types
// into c_string before using logger
// cString conversion helpers
// are provided in the helper library
// -----------------------------------

//#define DEV_ENV
#ifdef DEV_ENV
#define initLogger() initSerial()
#define info(x) Serial.println(x)
#define logSd(x) Serial.println(x)
#else
#define initLogger() initLcdSd()
#define info(x) outToLcd(x)
#define logSd(x) outToLcdSd(x)
#endif

#ifdef DEV_ENV
extern void initSerial();
#else
extern void initLcdSd();
extern void outToLcd(char *);
extern void outToLcdSd(char *);
#endif

#endif