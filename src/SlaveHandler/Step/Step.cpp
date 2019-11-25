#include "Step.h"

// --------------------------
// STEP PUBLIC METHODS
// --------------------------
// private
void Step::setAction(STEPACTION action)
{
    this->stepAction = action;
};

// public
Step::Step()
{
    this->setStep(DUMMY_STEP, "\0");
};

Step::Step(STEPACTION action)
{
    this->setStep(action, "\0");
};

Step::Step(STEPACTION action, char *target)
{
    this->setStep(action, target);
};

Step::~Step() {};

// setters
void Step::setStep(STEPACTION action, char *target)
{
    this->setAction(action);
    strcpy(this->stepTarget, target);
    this->setStatus(STEP_AWAITING_START);
};

void Step::setStatus(STEPSTATUS status)
{
    this->stepStatus = status;
};

void Step::resetStep()
{
    this->setStep(DUMMY_STEP, "\0");
};

// getters
char *Step::getDetails()
{
    static char stepDetails[DEFAULT_CHAR_ARRAY_SIZE];

    strcpy(stepDetails, stepActionToString[this->stepAction]);
    strcat(stepDetails, ", ");
    strcat(stepDetails, stepStatusToString[this->stepStatus]);
    if (strlen(this->stepTarget) > 0)
    {
        strcat(stepDetails, ", ");
        strcat(stepDetails, this->stepTarget);
    }
    return stepDetails;
};

char *Step::getSlaveString()
{
    static char slaveStr[DEFAULT_CHAR_ARRAY_SIZE];
    char stepActionEnumString[3];
    GET_TWO_DIGIT_STRING(stepActionEnumString, this->stepAction);
    strcpy(slaveStr, stepActionEnumString);
    if (strlen(this->stepTarget) > 0)
    {
        strcat(slaveStr, this->stepTarget);
    }
    return slaveStr;
};

STEPACTION Step::getStepAction()
{
    return this->stepAction;
};

STEPSTATUS Step::getStepStatus()
{
    return this->stepStatus;
};

char *Step::getStepTarget()
{
    return this->stepTarget;
};

// methods
bool Step::evaluateStep(char *input)
{
    // checks step target against current input.
    // toggle step to completion if target is reached
    if (this->stepAction == DUMMY_STEP)
        return false;
    if (strcmp(this->stepTarget, input) == 0)
    {
        // results match, step is complete
        this->setStatus(STEP_COMPLETED);
        return true;
    }
    return false;
};