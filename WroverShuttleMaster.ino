#include <string.h>
#include <stdio.h>
#include "./src/WcsHandler/Wcshandler.h"
#include "./src/Helper/helper.h"

char in[256];

void setup()
{
  Serial.begin(115200);
  Serial.println("initializing wcs handler");
  wcsHandler.init();
  Serial.println("wcs handler initialized");
}

void loop()
{
  wcsHandler.run();
}