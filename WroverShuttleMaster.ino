#include <Automaton.h>
#include "./src/Logger/Logger.h"
#include "./src/Helper/Helper.h"
#include "./src/WcsHandler/WcsHandler.h"
#include "./src/SlaveHandler/SlaveHandler.h"

#include "./src/Logger/EEPROM/ELog.h"

void setup()
{
  // initializes logger. Serial if dev_env, or lcd and sd if prod
  initLogger();
  info("Logger initialized");

  // initialize slave handler
  info("Start Initializing SlaveHandler");
  slaveHandler.init(&Serial);
  info("Slave Handler initialized");

  // initialize wcs handler, connects to the internet and the server
  info("Start Initializing WcsHandler");
  wcsHandler.init();
  info("wcsHandler");

  // initialize EEPROM to handle microchip echos
  info("Start Initializing EEPROM");
  beginEEPROM();
}

void loop()
{
  automaton.run();
  wcsHandler.run();
  slaveHandler.run();
}