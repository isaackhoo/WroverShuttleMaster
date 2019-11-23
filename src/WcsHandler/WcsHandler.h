#pragma once

#ifndef WCSHANDLER_H
#define WCSHANDLER_H

#include "../Helper/Helper.h"

// --------------------------
// Wcs Public Variables
// --------------------------
class WcsHandler;
extern WcsHandler wcsHandler;

// --------------------------
// Wcs Public Methods
// --------------------------
class WcsHandler
{
  private:
    char readBuffer[DEFAULT_CHAR_ARRAY_SIZE * 2];
    int read();
    void handle(int);

  public:
    void init(void);
    void run(void);
    bool send(char *);
};

#endif