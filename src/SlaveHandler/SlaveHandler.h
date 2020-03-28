#pragma once

#ifndef SLAVEHANDLER_H
#define SLAVEHANDLER_H

#include <HardwareSerial.h>
#include "./Step/Step.h"

// --------------------------------
// SLAVE HANDLER PRIVATE VARIABLES
// --------------------------------
const int MAX_STEP_SIZE = 20;

const int EXPECTED_POS_INST_LENGTH = 2;
const int EXPECTED_RETRIEVAL_INST_LENGTH = 6; // rrccbb
const int EXPECTED_STORAGE_INST_LENGTH = 6;   // rrccbb
const int RACK_ID_LENGTH = 2;
const int COLUMN_ID_LENGTH = 2;
const int BIN_POS_ID_LENGTH = 2;

// motor count constants
const int MOTORCOUNT_BUFFER_HOLE_TO_CENTER_OF_PILLAR = 135;
const int MOTORCOUNT_CENTER_OF_PILLAR_TO_ADAJ_SLOTHOLE_CENTER = 116;
const int MOTORCOUNT_BTWN_SLOTS_WITHIN_RAIL = 167;
const int MOTORCOUNT_PER_COLUMN = (2 * MOTORCOUNT_CENTER_OF_PILLAR_TO_ADAJ_SLOTHOLE_CENTER) + (5 * MOTORCOUNT_BTWN_SLOTS_WITHIN_RAIL);

// position const
const long POSITION_BUFFER = 0;
const long POSITION_IN_SHUTTLE_LIFTER = -415;
const long POSITION_SHUTTLE_BATTERY_CHANGE = -950;
const long POSITION_SHUTTLE_MAINTENANCE = 21150;
const long SHUTTLE_POSITION_MINIMUM = -999999;
const long SHUTTLE_POSITION_MAXIMUM = 999999;
const long positionToValue[10] = {POSITION_BUFFER, POSITION_IN_SHUTTLE_LIFTER, POSITION_SHUTTLE_BATTERY_CHANGE, POSITION_SHUTTLE_MAINTENANCE};
typedef enum
{
  BUFFER = 0,
  IN_LIFTER,
  BATTERY_CHANGE_AREA,
  MAINTENANCE_AREA,
  Num_Of_Positions
} POSITIONS;

// racking constants
const int COLUMN_WIDTH = 3000;
const int GAP_BETWEEN_COLUMN_AND_BIN = 100;
const int GAP_BETWEEN_BIN_WIDTH = 80;
const int BIN_WIDTH = 400;
const int BIN_LENGTH = 600;
const int GAP_BETWEEN_BIN_LENGTH = 30;
const int FIRST_DEPTH = 642;
const int SECOND_DEPTH = 1185;
const int BUFFER_DEPTH = 900;

// battery constants
const int BATT_VOLTS_LOWER_LIMIT = 235;

// others
const int STORAGE_BUFFER_DIRECTION = -1;
const int RETRIEVAL_BUFFER_DIRECTION = 1;
const int EXTEND_LEFT = -1;
const int EXTEND_RIGHT = 1;
const int FINGER_ID_0 = 0;
const int FINGER_ID_1 = 1;
const int ARM_HOME_POSITION = 0;
const int ARM_HOME_LEFT_MORE = -30;
const int ARM_HOME_RIGHT_MORE = 30;
const int CLEAR_TO_MOVE = 0;
const int BIN_PRESENT_IN_SLOT = 1;
const int BIN_ABSENT_IN_SLOT = 0;
typedef enum
{
  LEFT_COMPLETELY_EMPTY = 0,
  LEFT_SECOND_DEPTH_OCCUPIED,
  LEFT_BOTH_OCCUPIED,
  LEFT_BUFFER_OCCUPIED,
  RIGHT_COMPLETELY_EMPTY,
  RIGHT_SECOND_DEPTH_OCCUPIED,
  RIGHT_BOTH_OCCUPIED,
  RIGHT_BUFFER_OCCUPIED
} BINPOSSTATE;
static BINPOSSTATE binPosState;

typedef enum
{
  SHUTTLE_NOT_CLEAR_TO_MOVE = 0,
  SHUTTLE_CLEAR_TO_MOVE
} SHUTTLECLEARTOMOVE;
static SHUTTLECLEARTOMOVE shuttleClearToMove;

// echo
static const long SLAVE_ECHO_TIMEOUT_DURATION = 1000 * 5; // 5s timeout after sending out

// --------------------------------
// SLAVE HANDLER PUBLIC VARIABLES
// --------------------------------
class SlaveHandler;
extern SlaveHandler slaveHandler;

// --------------------------------
// SLAVE HANDLER PRIVATE METHODS
// --------------------------------

// --------------------------------
// SLAVE HANDLER PUBLIC METHODS
// --------------------------------
class SlaveHandler
{
private:
  // SlaveSerial ss;
  HardwareSerial *ss;
  char readString[DEFAULT_CHAR_ARRAY_SIZE];
  bool serialRead();
  bool serialWrite(char *);
  bool serialWrite(char *, bool);

  // steps handler
  Step steps[MAX_STEP_SIZE];

  int totalSteps = 0;
  void setTotalSteps(int);

  int currentStepIndex = 0;
  void incCurrentStepIndex();
  void resetCurrentStepIndex();

  bool overallStepsCompleted = false;
  void setOverallStepsCompleted(bool);

  // helper functions
  void getBinPosition(char *, char *, char *);
  int getArmExtensionDirection(char *);
  int getArmExtensionDepth(char *);
  void getPullingFingers(int, char *);
  void getPushingFingers(int, char *);

  // comms
  void handle();
  void reset();

  // slave echos
  char slaveEchoBuffer[DEFAULT_CHAR_ARRAY_SIZE];
  unsigned long slaveEchoTimeoutMillis;
  int slaveEchoRetries;

  bool setSlaveEcho(char *);
  char *getSlaveEcho();

  void setSlaveEchoTimeout();
  bool isSlaveEchoTimeout();
  bool clearSlaveEchoTimeout();
  bool confirmSlaveEcho();
  bool resetSlaveEcho();

  bool incSlaveEchoRetries();
  bool resetSlaveEchoRetries();
  int getSlaveEchoRetries();

public:
  SlaveHandler();
  void init(HardwareSerial *);
  void init(HardwareSerial *, int, int);
  void run();
  void beginNextStep();
  bool createStorageSteps(char *);
  bool createRetrievalSteps(char *);
  bool createMovementSteps(char *);
};

#endif