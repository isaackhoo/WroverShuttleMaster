#include "SlaveSerial.h"

SlaveSerial::SlaveSerial()
{
    this->ss = NULL;
};

SlaveSerial::SlaveSerial(HardwareSerial *serialPort)
{
    this->ss = serialPort;
};

SlaveSerial::~SlaveSerial(){};

void SlaveSerial::init(HardwareSerial *serialPort, char *name)
{
    if (!this->ss)
        this->ss = serialPort;
    this->init(name);
};

void SlaveSerial::init(char *name)
{
    if (strlen(name) > 0)
        strcpy(this->name, name);
    this->init();
};
void SlaveSerial::init()
{
    if (this->ss)
        this->ss->begin(DEFAULT_SERIAL_BAUD_RATE);
};

// setters
void SlaveSerial::resetReadString()
{
    this->readString[0] = '\0';
};

// getters
char *SlaveSerial::getReadString()
{
    return this->readString;
};

// methods
bool SlaveSerial::ready()
{
    if (!this->ss)
        return false;
    return true;
};

bool SlaveSerial::ssRead()
{
    if (this->ss->available() > 0)
    {
        this->resetReadString();
        String in = this->ss->readStringUntil('\n');
        in.trim();
        strcpy(readString, in.c_str());
        return true;
    }
    return false;
};

bool SlaveSerial::ssWrite(char *ssOut)
{
    if (strlen(ssOut) <= 0)
        return false;
    String strOut(ssOut);
    this->ss->println(strOut);
    delay(10); // give serial time to clear stream
    return true;
};