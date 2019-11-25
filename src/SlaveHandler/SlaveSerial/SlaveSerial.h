#ifndef SLAVESERIAL_H
#define SLAVESERIAL_H

#include "HardwareSerial.h"
#include "../../Helper/Helper.h"

class SlaveSerial
{
private:
    HardwareSerial *ss;
    char name[DEFAULT_CHAR_ARRAY_SIZE];
    char readString[DEFAULT_CHAR_ARRAY_SIZE];

public:
    SlaveSerial();
    SlaveSerial(HardwareSerial *);
    ~SlaveSerial();
    void init(HardwareSerial *, char *);
    void init(char *);
    void init();

    // setters
    void resetReadString();

    // getters
    char *getReadString();

    // methods
    bool ready();
    bool ssRead();
    bool ssWrite(char *);
};

#endif