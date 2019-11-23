#include "./src/Network/Network.h"
#include "./src/Logger/Logger.h"
#include "./src/Helper/Helper.h"

void setup()
{
  // put your setup code here, to run once:
  initLogger();

  // connect to the internet
  ConnectWifi();

  char *testing = "\x02Hello World\x03";
  info(testing);
  int pos = reverseFindIndex(testing, '\x03');
  info("searched for position index");
  char posStr[3];
  toCString(posStr, pos);
  info(posStr);
  info("done");
}

void loop()
{
// -----------------------------------
// Code to loop every cycle
// -----------------------------------
#ifndef DEV_ENV
  nextTimestampInterval();
#endif

}
