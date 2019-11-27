#include <Automaton.h>
#include "./src/Logger/Logger.h"
#include "./src/Helper/Helper.h"
#include "./src/WcsHandler/WcsHandler.h"
#include "./src/SlaveHandler/SlaveHandler.h"

void setup()
{
  // initializes logger. Serial if dev_env, or lcd and sd if prod
  initLogger();

  // initialize slave handler
  info("initialising slaveHandler");
  slaveHandler.init(&Serial);

  // initialize wcs handler, connects to the internet and the server
  info("initialising wcsHandler");
  wcsHandler.init();
}

void loop()
{
  automaton.run();
  wcsHandler.run();
  slaveHandler.run();
}