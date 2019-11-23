#include "./src/Network/Network.h"
#include "./src/Logger/Logger.h"
#include "./src/Helper/Helper.h"

void setup()
{
  // put your setup code here, to run once:
  initLogger();

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
  // put your main code here, to run repeatedly:
}
