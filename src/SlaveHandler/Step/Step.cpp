#include <string.h>
#include <stdio.h>
#include "../../Logger/Logger.h"
#include "../../Helper/Helper.h"
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

Step::~Step(){};

// setters
void Step::setStep(STEPACTION action, char *target)
{
    this->setAction(action);
    // strcpy_s(this->stepTarget, sizeof this->stepTarget, target);
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

    // strcpy_s(stepDetails, sizeof stepDetails, stepActionToString[this->stepAction]);
    // strcat_s(stepDetails, sizeof stepDetails, ", ");
    // strcat_s(stepDetails, sizeof stepDetails, stepStatusToString[this->stepStatus]);

    strcpy(stepDetails, stepActionToString[this->stepAction]);
    strcat(stepDetails, ", ");
    strcat(stepDetails, stepStatusToString[this->stepStatus]);
    if (strlen(this->stepTarget) > 0)
    {
        // strcat_s(stepDetails, sizeof stepDetails, ", ");
        // strcat_s(stepDetails, sizeof stepDetails, this->stepTarget);

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
    // strcpy_s(slaveStr, sizeof slaveStr, stepActionEnumString);
    strcpy(slaveStr, stepActionEnumString);
    if (strlen(this->stepTarget) > 0)
    {
        // strcat_s(slaveStr, sizeof slaveStr, this->stepTarget);

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

char *Step::getStepErrorDetails()
{
    return this->stepErrorDetails;
};

// methods
bool Step::evaluateStep(char *input)
{
    // checks step target against current input.
    // toggle step to completion if target is reached
    if (this->stepAction == DUMMY_STEP)
        return false;

    // check input for any errors
    if (input[0] == NAK[0])
    {
        // error encountered
        this->setStatus(STEP_ERROR);
        strcpy(this->stepErrorDetails, input);
        return true;
    }
    else
    {
        int evaluationResult = strcmp(this->stepTarget, input);
        if (evaluationResult == 0)
        {
            // results match, step is complete
            this->setStatus(STEP_COMPLETED);
            return true;
        }
    }
    return false;
};