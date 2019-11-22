#include "Network.h"
#include "Helper.h"

void setup()
{
  // put your setup code here, to run once:
  initPrinter();
  // ConnectWifi();
  // ConnectTcpServer();

  char* test = "\x02Hello there\x03";
  int pos = reverseFindIndex(test, '\x03');
  char* poscstr = "testing";
  Serial.println(poscstr);
  info(poscstr);
}

void loop()
{
  // put your main code here, to run repeatedly:

}
