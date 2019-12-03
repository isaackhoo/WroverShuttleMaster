#pragma once

#ifndef STEP_H
#define STEP_H

#include "../../Helper/Helper.h"

// --------------------------
// STEP PRIVATE VARIABLES
// --------------------------
static const char *NAK = "\x21";

// --------------------------
// STEP PUBLIC VARIABLES
// --------------------------
typedef enum
{
    DUMMY_STEP = 0,
    CHECK_CLEAR_TO_MOVE,
    MOVE_TO_BIN,
    MOVE_TO_POS,
    MOVE_TO_FROM_LIFTER,
    CHECK_RACK_BIN_SLOT,
    CHECK_BUFFER_BIN_SLOT,
    EXTEND_ARM,
    RETRACT_ARM,
    EXTEND_FINGERS,
    RETRACT_FINGERS,
    ADJUST_POS,
    CHECK_FIRST_DEPTH_BIN_SLOT,
    CHECK_SECOND_DEPTH_BIN_SLOT,
    Num_Of_Step_Actions
} STEPACTION;
const char stepActionToString[][28] = {"Dummy", "Check clear", "Move to Bin",
                                       "Move to Pos", "Move to/from Lifter",
                                       "Check Rack Slot", "Check Buffer Slot",
                                       "Extend Arm", "Retract Arm", "Extend Fingers",
                                       "Retract Fingers", "Adjust Pos",
                                       "Check 1D slot", "Check 2D slot"};
typedef enum
{
    STEP_AWAITING_START = 0,
    STEP_PENDING,
    STEP_COMPLETED,
    STEP_ERROR,
    Num_Of_Step_Status
} STEPSTATUS;
const char stepStatusToString[][20] = {"Awaiting start", "Pending", "Completed"};

// --------------------------
// STEP PRIVATE METHODS
// --------------------------

// --------------------------
// STEP PUBLIC METHODS
// --------------------------
class Step
{
  private:
    STEPACTION stepAction;
    STEPSTATUS stepStatus;
    char stepTarget[DEFAULT_CHAR_ARRAY_SIZE];
    char stepErrorDetails[DEFAULT_CHAR_ARRAY_SIZE];

    void setAction(STEPACTION);

  public:
    Step();
    Step(STEPACTION);
    Step(STEPACTION, char *);
    ~Step();

    // setters
    void setStep(STEPACTION, char *);
    void setStatus(STEPSTATUS);
    void resetStep();

    // getters
    char *getDetails();
    char *getSlaveString();
    STEPACTION getStepAction();
    STEPSTATUS getStepStatus();
    char *getStepTarget();
    char *getStepErrorDetails();

    // methods
    bool evaluateStep(char *);
};

#endif
