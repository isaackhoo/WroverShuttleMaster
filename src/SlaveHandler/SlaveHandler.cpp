#include <String.h>
#include <string.h>
#include <stdio.h>
#include "SlaveHandler.h"
#include "../WcsHandler/WcsHandler.h"
#include "../Helper/Helper.h"
#include "../Status/Status.h"
#include "../Logger/Logger.h"

// --------------------------------
// SLAVE HANDLER PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// SLAVE HANDLER PUBLIC VARIABLES
// --------------------------------
SlaveHandler slaveHandler;

// --------------------------------
// SLAVE HANDLER PRIVATE METHODS
// --------------------------------
bool SlaveHandler::serialRead()
{
    if (this->ss->available() > 0)
    {
        String in = this->ss->readStringUntil('\n');
        in.trim();

        // check if its an echo reply
        if (strncmp(this->getSlaveEcho(), in.c_str(), strlen(this->getSlaveEcho())) == 0)
        {
            // is echo
            char slaveEchoReply[DEFAULT_CHAR_ARRAY_SIZE];
            sprintf(slaveEchoReply, "%s slave echo received", this->getSlaveEcho());
            // clear echo
            this->confirmSlaveEcho();
            // return false to main since this is not something to process.
            return false;
        }

        // process as per normal if not echo
        if (strlen(this->readString) > 0)
            strcat(this->readString, in.c_str());
        else
            strcpy(this->readString, in.c_str());

        // remove newline character
        if (this->readString[strlen(this->readString) - 2] == '\r')
            this->readString[strlen(this->readString) - 2] = '\0';
        if (this->readString[strlen(this->readString) - 2] == '\n')
            this->readString[strlen(this->readString) - 2] = '\0';
        if (this->readString[strlen(this->readString) - 1] == '\n')
            this->readString[strlen(this->readString) - 1] = '\0';

        info("read from slave serial");
        info(this->readString);

        return true;
    }
    return false;
};

bool SlaveHandler::serialWrite(char *slaveInst)
{
    // if (strlen(slaveInst) <= 0)
    //     return false;
    // logSd(slaveInst);
    // return this->ss->println(slaveInst);
    return this->serialWrite(slaveInst, true);
};

bool SlaveHandler::serialWrite(char *slaveInst, bool awaitEcho)
{
    if (strlen(slaveInst) <= 0)
        return false;
    logSd(slaveInst);
    this->ss->println(slaveInst);

    if (awaitEcho)
    {
        char slaveEchoLog[DEFAULT_CHAR_ARRAY_SIZE];
        sprintf(slaveEchoLog, "%s expecting slave echo", slaveInst);
        info(slaveEchoLog);
        // save echo to echo buffer
        this->setSlaveEcho(slaveInst);
        // set echo reply timeout
        this->setSlaveEchoTimeout();
    }
};

void SlaveHandler::setTotalSteps(int total)
{
    this->totalSteps = total;
};

void SlaveHandler::incCurrentStepIndex()
{
    this->currentStepIndex++;
    char currentStepInfo[DEFAULT_CHAR_ARRAY_SIZE];
    sprintf(currentStepInfo, "Step Index inc to %i", this->currentStepIndex);
    info(currentStepInfo);
};

void SlaveHandler::resetCurrentStepIndex()
{
    this->currentStepIndex = 0;
};

void SlaveHandler::setOverallStepsCompleted(bool isComplete)
{
    this->overallStepsCompleted = isComplete;
    if (this->overallStepsCompleted)
        logSd("Completed overall steps");
};

void SlaveHandler::getBinPosition(char *inCol, char *binInColPos, char *output)
{
    // calculates the position of the bin relative to buffer position 0
    // buffer is a column 0
    // first bin column starts at 1.
    // gap between column and bin is different from gap between bin and bin

    long pos = 0;

    int col = atoi(inCol);       // convert col char to long
    int bin = atoi(binInColPos); // convert bin char to long

    // total column distance to move
    // pos += (col - 1) * COLUMN_WIDTH;

    // calcualte bin distance to move within column
    // note that distance of first bin from column is different
    //      |               |   |               |   |
    //      |               |   |               |   |
    //      |               |   |               |   |
    //      |      Bin      |   |      Bin      |   |
    //      |               |   |               |   |
    //      |               |   |               |   |
    //      | ---  400 ---  |   |  --- 400 ---  |   |
    //-100 -|_______________| 80|_______________| 80|__ ...

    // get just the bin column
    // since bins are position and numbered as such
    //

    //  02   04   06   08   10   12
    //
    //  01   03   05   07   09   11

    int binCol = ((bin + 1) / 2); //  (1) + 1 / 2 = 1; (2) + 1 / 2 = 1; (3) + 1 / 2 = 2 etc.. Not using this atm...

    pos += MOTORCOUNT_BUFFER_HOLE_TO_CENTER_OF_PILLAR;                                                                 // for buffer
    pos += ((col - 1) * MOTORCOUNT_PER_COLUMN);                                                                        // for full columns
    pos += (MOTORCOUNT_CENTER_OF_PILLAR_TO_ADAJ_SLOTHOLE_CENTER + ((binCol - 1) * MOTORCOUNT_BTWN_SLOTS_WITHIN_RAIL)); // for in between bin slots

    /*pos += A;                            // for const buffer area to first pillar
    pos += (col - 1) * D;                  // for whole columns
    pos += B + (((bin + 1) / 2) - 1) * C) // for bin in col

    // pos += GAP_BETWEEN_COLUMN_AND_BIN; // for first bin
    // pos += binCol * (GAP_BETWEEN_BIN_WIDTH + BIN_WIDTH);
    // pos += BIN_WIDTH / 2; // brings shuttle to center of bin column
    */

    char posStr[21];
    toCString(posStr, pos);
    // strcpy_s(output, sizeof output, posStr);
    strcpy(output, posStr);
};

int SlaveHandler::getArmExtensionDirection(char *inRack)
{
    // get direction to extend arms to
    //    Rack|     |Rack     Rack|     |Rack
    //  06  05|     |05  06 06  05|     |05  06
    //  04  03|     |03  04 04  03|     |03  04
    //  02  01|     |01  02 02  01|     |01  02
    //  --01--|     |--02-- --01--|     |--02--
    //  12  11|     |11  12 12  11|     |11  12
    //  10  09|     |09  10 10  09|     |09  10
    //  08  07|     |07  08 08  07|     |07  08
    //  06  05|     |05  06 06  05|     |05  06
    //  04  03|     |03  04 04  03|     |03  04
    //  02  01|     |01  02 02  01|     |01  02

    // get extension direction
    int rackInt = atoi(inRack);
    int direction = rackInt % 2;
    int extensionDirection = 0;
    if (direction == 1)
    {
        // extend left
        extensionDirection = EXTEND_LEFT;
    }
    else
    {
        // extend right
        extensionDirection = EXTEND_RIGHT;
    }

    return extensionDirection;
};

int SlaveHandler::getArmExtensionDepth(char *binInColPos)
{
    // get extension depth
    int binInt = atoi(binInColPos);
    int binDepth = binInt % 2;
    int depth = 0;
    if (binDepth == 1)
    {
        // first depth
        depth = FIRST_DEPTH;
    }
    else
    {
        depth = SECOND_DEPTH;
    }

    return depth;
};

void SlaveHandler::getPullingFingers(int armExtensionDirection, char *output)
{
    //   ___________________________
    //  |                           |
    //
    //  0          Shuttle          1
    //
    //  |___________________________|
    //

    if (armExtensionDirection == EXTEND_LEFT || armExtensionDirection == STORAGE_BUFFER_DIRECTION)
    {
        itoa(FINGER_ID_0, output, 10);
    }
    else if (armExtensionDirection == EXTEND_RIGHT)
    {
        itoa(FINGER_ID_1, output, 10);
    }
};

void SlaveHandler::getPushingFingers(int armExtensionDirection, char *output)
{
    //   ___________________________
    //  |                           |
    //
    //  0          Shuttle          1
    //
    //  |___________________________|
    //

    // due to design. Finger 0 will always wear out faster than finger 1

    if (armExtensionDirection == EXTEND_LEFT)
    {
        itoa(FINGER_ID_1, output, 10);
    }
    else if (armExtensionDirection == EXTEND_RIGHT || armExtensionDirection == RETRIEVAL_BUFFER_DIRECTION)
    {
        itoa(FINGER_ID_0, output, 10);
    }
};

void SlaveHandler::handle()
{
    // if shuttle is not in active status, it is receiving delayed
    // responses. ignore these.
    if (status.getState() == IDLE)
        return;

    // handle slave reply
    bool res = steps[this->currentStepIndex].evaluateStep(this->readString);

    if (res)
    {
        // check if step has encountered any errors
        if (steps[this->currentStepIndex].getStepStatus() == STEP_ERROR)
        {
            // terminate other steps, jump to notifying wcs
            this->setOverallStepsCompleted(true);
        }
        else
        {
            // step is complete
            steps[this->currentStepIndex].setStatus(STEP_COMPLETED);
            // update status of shuttle based on step that is completed
            switch (steps[this->currentStepIndex].getStepAction())
            {
            case MOVE_TO_BIN:
            case MOVE_TO_POS:
            {
                status.setPos(atol(steps[this->currentStepIndex].getStepTarget()));
                break;
            }
            case EXTEND_ARM:
            {
                status.setIsCarryingBin(false);
                break;
            }
            case RETRACT_ARM:
            {
                status.setIsCarryingBin(true);
                break;
            }
            case EXTEND_FINGERS:
            {
                status.setIsFingerExtended(true);
                break;
            }
            case RETRACT_FINGERS:
            {
                status.setIsFingerExtended(false);
                break;
            }
            case BATT_VOLTS_READ:
            {
                // status.set??? what is this for, all the sets.. doesnt seem to do anything yet?
                // ** yupp doesnt do anything yet. Supposed to track every last shuttle action.
                // allow shuttle to resume from last action if it stops half way. -zac

                break;
            }
            default:
                break;
            }
            info("Step done");

            // check for more steps to do
            if (this->currentStepIndex < this->totalSteps - 1)
                this->incCurrentStepIndex();
            else
                this->setOverallStepsCompleted(true);
        }
    }
    // do nothing if step is not complete.
    // instructions will be resent to slave.

    // check for more steps to perform
    if (this->overallStepsCompleted == false)
    {
        // there are more steps to do
        if (steps[this->currentStepIndex].getStepAction() != DUMMY_STEP)
        {
            this->beginNextStep();
        }
    }
    else
    {
        if (steps[this->currentStepIndex].getStepStatus() == STEP_ERROR)
        {
            // notify wcs of error in step
            wcsHandler.sendJobCompletionNotification(status.getActionEnum(), steps[this->currentStepIndex].getStepErrorDetails());
            status.setState(SHUTTLE_ERROR);
        }
        else
        {
            // job completed
            // set status to idle
            status.setState(IDLE);
            delay(50);
            // update wcs on job completion
            wcsHandler.sendJobCompletionNotification(status.getActionEnum(), "01");
        }

        // reset slave handler
        this->reset();
    }

    // clear serial read string
    this->readString[0] = '\0';
};

void SlaveHandler::reset()
{
    for (int i = 0; i < this->totalSteps; i++)
    {
        this->steps[i].resetStep();
    }
    // reset current step index
    this->setTotalSteps(0);
    this->resetCurrentStepIndex();
    this->setOverallStepsCompleted(false);
};

// Slave Echo
bool SlaveHandler::setSlaveEcho(char *echo)
{
    strcpy(this->slaveEchoBuffer, echo);
    return true;
};
char *SlaveHandler::getSlaveEcho()
{
    return this->slaveEchoBuffer;
};
bool SlaveHandler::resetSlaveEcho()
{
    this->slaveEchoBuffer[0] = '\0';
    return true;
};

void SlaveHandler::setSlaveEchoTimeout()
{
    this->slaveEchoTimeoutMillis = millis() + SLAVE_ECHO_TIMEOUT_DURATION;
};
bool SlaveHandler::isSlaveEchoTimeout()
{
    if (this->slaveEchoTimeoutMillis <= 0)
        return false;
    if (millis() >= this->slaveEchoTimeoutMillis)
        return true;
    return false;
};
bool SlaveHandler::clearSlaveEchoTimeout()
{
    this->slaveEchoTimeoutMillis = 0;
    return true;
};

bool SlaveHandler::incSlaveEchoRetries()
{
    this->slaveEchoRetries++;
    return true;
};
bool SlaveHandler::resetSlaveEchoRetries()
{
    this->slaveEchoRetries = 0;
    return true;
};
int SlaveHandler::getSlaveEchoRetries()
{
    return this->slaveEchoRetries;
};

bool SlaveHandler::confirmSlaveEcho()
{
    this->resetSlaveEcho();
    this->clearSlaveEchoTimeout();
    this->resetSlaveEchoRetries();
};

// --------------------------------
// SLAVE HANDLER PUBLIC METHODS
// --------------------------------
SlaveHandler::SlaveHandler()
{
    // intialize slave serial variables
    this->readString[0] = '\0';

    // initialize slave echo variables
    this->slaveEchoBuffer[0] = '\0';
    this->slaveEchoRetries = 0;
}

void SlaveHandler::init(HardwareSerial *serialPort)
{
    this->ss = serialPort;
    this->ss->end();
    this->ss->begin(DEFAULT_SERIAL_BAUD_RATE);
    info("Slave Serial started");
};

void SlaveHandler::run()
{
    // check for echo timeout
    if (this->isSlaveEchoTimeout())
    {
        // inc retries count
        this->incSlaveEchoRetries();
        // clear timeout
        this->clearSlaveEchoTimeout();
        info("increased slave echo retries count");

        if (this->getSlaveEchoRetries() <= 5)
        {
            info("Resending unreplied slave echo");
            // resend
            this->serialWrite(this->getSlaveEcho(), true);
        }
        else
        {
            logSd("Failed 5 slave echo retries.");
        }
    }

    // checks slave serial for input
    if (this->serialRead())
    {
        this->handle();
    }
};

void SlaveHandler::beginNextStep()
{
    char beginStepInfo[DEFAULT_CHAR_ARRAY_SIZE];
    sprintf(beginStepInfo, "Starting: %s", steps[this->currentStepIndex].getDetails());
    logSd(beginStepInfo);
    // set shuttle status to active
    status.setActiveState();
    // change step status to active/pending
    steps[this->currentStepIndex].setStatus(STEP_PENDING);
    logSd("Step status set to pending");
    // send instructions over slave serial
    logSd("Writing to slave serial");
    this->serialWrite(steps[this->currentStepIndex].getSlaveString());
};

bool SlaveHandler::createStorageSteps(char *storageInst)
{
    info("Creating storage steps for");
    info(storageInst);

    // interpret instructions accordingly
    // use rack id to identify arm extension direction
    // use column id to identify primary shuttle movement
    // use bin pos to identify secondary shuttle movement
    // also use bin pos to identify depth of arm extension

    if (strlen(storageInst) != EXPECTED_STORAGE_INST_LENGTH)
        return false;

    bool successfullyProcessed = true;

    char inRack[DEFAULT_CHAR_ARRAY_SIZE];
    char inCol[DEFAULT_CHAR_ARRAY_SIZE];
    char binInColPos[DEFAULT_CHAR_ARRAY_SIZE];

    successfullyProcessed = strcut(inRack, storageInst, 0, RACK_ID_LENGTH);
    successfullyProcessed = strcut(inCol, storageInst, 0, COLUMN_ID_LENGTH);
    successfullyProcessed = strcut(binInColPos, storageInst, 0, BIN_POS_ID_LENGTH);

    if (!successfullyProcessed)
        return false;

    // clear to move arr
    char clearToMoveStorArr[DEFAULT_CHAR_ARRAY_SIZE];
    shuttleClearToMove = SHUTTLE_CLEAR_TO_MOVE;
    itoa(shuttleClearToMove, clearToMoveStorArr, 10);

    // get batt volts limit
    char battVoltsLowerLimit[DEFAULT_CHAR_ARRAY_SIZE];
    itoa(BATT_VOLTS_LOWER_LIMIT, battVoltsLowerLimit, 10);

    // get char of buffer position
    char bufferPosArr[DEFAULT_CHAR_ARRAY_SIZE];
    toCString(bufferPosArr, positionToValue[BUFFER]);

    // get storage buffer depth
    int storageBufferDepth = BUFFER_DEPTH * STORAGE_BUFFER_DIRECTION;

    // check bin in buffer
    char binInBufferSlotArr[DEFAULT_CHAR_ARRAY_SIZE];
    if (storageBufferDepth < 0) // LEFT
        binPosState = LEFT_BUFFER_OCCUPIED;
    else if (storageBufferDepth > 0) // RIGHT
        binPosState = RIGHT_BUFFER_OCCUPIED;
    GET_TWO_DIGIT_STRING(binInBufferSlotArr, binPosState);

    // storage buffer depth char
    char storageBufferDepthArr[DEFAULT_CHAR_ARRAY_SIZE];
    itoa(storageBufferDepth, storageBufferDepthArr, 10);

    // get pulling fingers
    char pullingFingersArr[DEFAULT_CHAR_ARRAY_SIZE];
    this->getPullingFingers(STORAGE_BUFFER_DIRECTION, pullingFingersArr);

    // so fingers do not get stuck at bin
    char armHomeMore[DEFAULT_CHAR_ARRAY_SIZE];
    itoa(ARM_HOME_RIGHT_MORE, armHomeMore, 10);

    // check bin has indeed been ret from buffer
    char checkBinTakenFromBufferSlotArr[DEFAULT_CHAR_ARRAY_SIZE];
    if (storageBufferDepth < 0) // LEFT
        binPosState = LEFT_COMPLETELY_EMPTY;
    else if (storageBufferDepth > 0) // RIGHT
        binPosState = RIGHT_COMPLETELY_EMPTY;
    GET_TWO_DIGIT_STRING(checkBinTakenFromBufferSlotArr, binPosState);

    // get arm home char
    char armHomeArr[DEFAULT_CHAR_ARRAY_SIZE];
    itoa(ARM_HOME_POSITION, armHomeArr, 10);

    // get position that shuttle should move to
    char binPosArr[DEFAULT_CHAR_ARRAY_SIZE];
    this->getBinPosition(inCol, binInColPos, binPosArr);

    // get extension direction & depth
    int extensionDirection = this->getArmExtensionDirection(inRack);
    int depth = this->getArmExtensionDepth(binInColPos);
    // convert extension to char
    char extensionResultArr[DEFAULT_CHAR_ARRAY_SIZE];
    itoa((depth * extensionDirection), extensionResultArr, 10);

    // get check slot empty array
    char noBinInStorageSlotArr[DEFAULT_CHAR_ARRAY_SIZE];
    if (extensionDirection == EXTEND_LEFT)
    {
        if (depth == FIRST_DEPTH)
            binPosState = LEFT_SECOND_DEPTH_OCCUPIED;
        else if (depth == SECOND_DEPTH)
            binPosState = LEFT_COMPLETELY_EMPTY;
    }
    else if (extensionDirection == EXTEND_RIGHT)
    {
        if (depth == FIRST_DEPTH)
            binPosState = RIGHT_SECOND_DEPTH_OCCUPIED;
        else if (depth == SECOND_DEPTH)
            binPosState = RIGHT_COMPLETELY_EMPTY;
    }
    GET_TWO_DIGIT_STRING(noBinInStorageSlotArr, binPosState);

    // get pushing fingers
    char pushingFingersArr[DEFAULT_CHAR_ARRAY_SIZE];
    this->getPushingFingers(extensionDirection, pushingFingersArr);

    // get check bin stored in correct position
    char checkBinStoredInStorageSlotArr[DEFAULT_CHAR_ARRAY_SIZE];
    if (extensionDirection == EXTEND_LEFT)
    {
        if (depth == FIRST_DEPTH)
            binPosState = LEFT_BOTH_OCCUPIED; // store to left 1st depth, both occupied
        else if (depth == SECOND_DEPTH)
            binPosState = LEFT_SECOND_DEPTH_OCCUPIED; // store to left 2nd depth, 2nd depth occupied
    }
    else if (extensionDirection == EXTEND_RIGHT)
    {
        if (depth == FIRST_DEPTH)
            binPosState = RIGHT_BOTH_OCCUPIED; // store to right 1st depth, both occupied
        else if (depth == SECOND_DEPTH)
            binPosState = RIGHT_SECOND_DEPTH_OCCUPIED; // store to right 2nd depth, 2nd depth occupied
    }
    GET_TWO_DIGIT_STRING(checkBinStoredInStorageSlotArr, binPosState);

    //steps[0].setStep(CHECK_CLEAR_TO_MOVE, clearToMoveStorArr);
    //steps[0].setStep(BATT_VOLTS_READ, battVoltsLowerLimit);
    steps[0].setStep(MOVE_TO_POS, bufferPosArr);
    steps[1].setStep(CHECK_BUFFER_BIN_SLOT, binInBufferSlotArr);
    steps[2].setStep(EXTEND_ARM, storageBufferDepthArr);
    steps[3].setStep(EXTEND_FINGERS, pullingFingersArr);
    steps[4].setStep(RETRACT_ARM, armHomeMore);
    steps[5].setStep(RETRACT_ARM, armHomeArr);
    steps[6].setStep(RETRACT_FINGERS, pullingFingersArr);
    steps[7].setStep(CHECK_BUFFER_BIN_SLOT, checkBinTakenFromBufferSlotArr); //
    steps[8].setStep(CHECK_CLEAR_TO_MOVE, clearToMoveStorArr);
    steps[9].setStep(MOVE_TO_BIN, binPosArr);
    steps[10].setStep(CHECK_RACK_BIN_SLOT, noBinInStorageSlotArr);
    steps[11].setStep(EXTEND_FINGERS, pushingFingersArr);
    steps[12].setStep(EXTEND_ARM, extensionResultArr);
    steps[13].setStep(RETRACT_ARM, armHomeArr);
    steps[14].setStep(RETRACT_FINGERS, pushingFingersArr);
    steps[15].setStep(CHECK_RACK_BIN_SLOT, checkBinStoredInStorageSlotArr); //

    this->setOverallStepsCompleted(false);
    this->setTotalSteps(16);

    // for (int i = 0; i < this->totalSteps; i++)
    // {
    //     char storageStepsInfo[DEFAULT_CHAR_ARRAY_SIZE];
    //     sprintf(storageStepsInfo, "Step %d: %s", i, steps[i].getDetails());
    //     info(storageStepsInfo);
    // }

    return true;
};

bool SlaveHandler::createRetrievalSteps(char *retrievalInst)
{
    info("Creating retrieval steps for");
    info(retrievalInst);
    // interpret instructions accordingly
    // use rack id to identify arm extension direction
    // use column id to identify primary shuttle movement
    // use bin pos to identify secondary shuttle movement
    // also use bin pos to identify depth of arm extension

    if (strlen(retrievalInst) != EXPECTED_RETRIEVAL_INST_LENGTH)
        return false;

    bool successfullyProcessed = true;

    char inRack[DEFAULT_CHAR_ARRAY_SIZE];
    char inCol[DEFAULT_CHAR_ARRAY_SIZE];
    char binInColPos[DEFAULT_CHAR_ARRAY_SIZE];

    successfullyProcessed = strcut(inRack, retrievalInst, 0, RACK_ID_LENGTH);
    successfullyProcessed = strcut(inCol, retrievalInst, 0, COLUMN_ID_LENGTH);
    successfullyProcessed = strcut(binInColPos, retrievalInst, 0, BIN_POS_ID_LENGTH);

    if (!successfullyProcessed)
        return false;

    // clear to move arr
    char clearToMoveArr[DEFAULT_CHAR_ARRAY_SIZE];
    shuttleClearToMove = SHUTTLE_CLEAR_TO_MOVE;
    itoa(shuttleClearToMove, clearToMoveArr, 10);

    // get batt volts limit
    char battVoltsLowerLimit[DEFAULT_CHAR_ARRAY_SIZE];
    itoa(BATT_VOLTS_LOWER_LIMIT, battVoltsLowerLimit, 10);

    // get position that shuttle should move to
    char binPosArr[DEFAULT_CHAR_ARRAY_SIZE];
    this->getBinPosition(inCol, binInColPos, binPosArr); // bin pos

    // arm extension direction and depth
    int extensionDirection = getArmExtensionDirection(inRack);
    int depth = getArmExtensionDepth(binInColPos);
    // convert extension to char
    char extensionResultArr[DEFAULT_CHAR_ARRAY_SIZE];
    itoa((depth * extensionDirection), extensionResultArr, 10);
    // check rack bin slot
    char binInRackSlotArr[DEFAULT_CHAR_ARRAY_SIZE];
    if (extensionDirection == EXTEND_LEFT)
    {
        if (depth == FIRST_DEPTH)
            binPosState = LEFT_BOTH_OCCUPIED;
        else if (depth == SECOND_DEPTH)
            binPosState = LEFT_SECOND_DEPTH_OCCUPIED;
    }
    else if (extensionDirection == EXTEND_RIGHT)
    {
        if (depth == FIRST_DEPTH)
            binPosState = RIGHT_BOTH_OCCUPIED;
        else if (depth == SECOND_DEPTH)
            binPosState = RIGHT_SECOND_DEPTH_OCCUPIED;
    }
    GET_TWO_DIGIT_STRING(binInRackSlotArr, binPosState);

    // get finger to pull bin into shuttle
    char pullingFingersId[DEFAULT_CHAR_ARRAY_SIZE];
    this->getPullingFingers(extensionDirection, pullingFingersId);

    // arm to over compensate so fingers are not stuck
    char armHomeMore[DEFAULT_CHAR_ARRAY_SIZE];
    if (extensionDirection == EXTEND_LEFT)
    {
        itoa(ARM_HOME_RIGHT_MORE, armHomeMore, 10);
    }
    else if (extensionDirection == EXTEND_RIGHT)
    {
        itoa(ARM_HOME_LEFT_MORE, armHomeMore, 10);
    }

    // arm home pos to char
    char armHomeArr[DEFAULT_CHAR_ARRAY_SIZE];
    itoa(ARM_HOME_POSITION, armHomeArr, 10);

    // check if bin was retrieved
    char checkBinInRackRetrievedArr[DEFAULT_CHAR_ARRAY_SIZE];
    if (extensionDirection == EXTEND_LEFT)
    {
        if (depth == FIRST_DEPTH)
            binPosState = LEFT_SECOND_DEPTH_OCCUPIED; // retrieve 1st depth, 2nd depth there
        else if (depth == SECOND_DEPTH)
            binPosState = LEFT_COMPLETELY_EMPTY; // retrieve 2nd depth, left empty
    }
    else if (extensionDirection == EXTEND_RIGHT)
    {
        if (depth == FIRST_DEPTH)
            binPosState = RIGHT_SECOND_DEPTH_OCCUPIED; // retrieve 1st depth, 2nd depth there
        else if (depth == SECOND_DEPTH)
            binPosState = RIGHT_COMPLETELY_EMPTY; // retrieve 2nd depth, right empty
    }
    GET_TWO_DIGIT_STRING(checkBinInRackRetrievedArr, binPosState);

    // get buffer pos in char
    char bufferPosArr[DEFAULT_CHAR_ARRAY_SIZE];
    toCString(bufferPosArr, positionToValue[BUFFER]);

    // get retrieval buffer depth
    int retrievalBufferDepth = BUFFER_DEPTH * RETRIEVAL_BUFFER_DIRECTION;

    // finger to push bin into retrieval buffer
    char pushingFingersArr[DEFAULT_CHAR_ARRAY_SIZE];
    this->getPushingFingers(RETRIEVAL_BUFFER_DIRECTION, pushingFingersArr);

    // get release bufer bin depth
    char retrievalBufferArr[DEFAULT_CHAR_ARRAY_SIZE];
    itoa(retrievalBufferDepth, retrievalBufferArr, 10);

    // check bin in buffer slot
    char noBinInRetrievalBufferArr[DEFAULT_CHAR_ARRAY_SIZE];
    if (retrievalBufferDepth < 0) // LEFT
        binPosState = LEFT_COMPLETELY_EMPTY;
    else if (retrievalBufferDepth > 0) // RIGHT
        binPosState = RIGHT_COMPLETELY_EMPTY;
    GET_TWO_DIGIT_STRING(noBinInRetrievalBufferArr, binPosState);

    // check bin indeed has been pushed to buffer slot
    char checkBinPushedToBufferSlotArr[DEFAULT_CHAR_ARRAY_SIZE];
    if (retrievalBufferDepth < 0) // LEFT
        binPosState = LEFT_BUFFER_OCCUPIED;
    else if (retrievalBufferDepth > 0) // RIGHT
        binPosState = RIGHT_BUFFER_OCCUPIED;
    GET_TWO_DIGIT_STRING(checkBinPushedToBufferSlotArr, binPosState);

    //steps[0].setStep(CHECK_CLEAR_TO_MOVE, clearToMoveArr);
    //steps[0].setStep(BATT_VOLTS_READ, battVoltsLowerLimit);
    steps[0].setStep(MOVE_TO_BIN, binPosArr);
    steps[1].setStep(CHECK_RACK_BIN_SLOT, binInRackSlotArr);
    steps[2].setStep(EXTEND_ARM, extensionResultArr);
    steps[3].setStep(EXTEND_FINGERS, pullingFingersId);
    steps[4].setStep(RETRACT_ARM, armHomeMore);
    steps[5].setStep(RETRACT_ARM, armHomeArr);
    steps[6].setStep(RETRACT_FINGERS, pullingFingersId);
    steps[7].setStep(CHECK_RACK_BIN_SLOT, checkBinInRackRetrievedArr);
    steps[8].setStep(CHECK_CLEAR_TO_MOVE, clearToMoveArr);
    steps[9].setStep(MOVE_TO_POS, bufferPosArr);
    steps[10].setStep(CHECK_BUFFER_BIN_SLOT, noBinInRetrievalBufferArr);
    steps[11].setStep(EXTEND_FINGERS, pushingFingersArr);
    steps[12].setStep(EXTEND_ARM, retrievalBufferArr);
    steps[13].setStep(RETRACT_ARM, armHomeArr);
    steps[14].setStep(RETRACT_FINGERS, pushingFingersArr);
    steps[15].setStep(CHECK_BUFFER_BIN_SLOT, checkBinPushedToBufferSlotArr);

    // set overall completion to false
    this->setOverallStepsCompleted(false);
    this->setTotalSteps(16);

    // for (int i = 0; i < this->totalSteps; i++)
    // {
    //     char retrievalStepsInfo[DEFAULT_CHAR_ARRAY_SIZE];
    //     sprintf(retrievalStepsInfo, "Step %d: %s", i, steps[i].getDetails());
    //     info(retrievalStepsInfo);
    // }

    return true;
};

bool SlaveHandler::createMovementSteps(char *movementInst)
{
    info("Creating movement steps for");
    info(movementInst);

    if (strlen(movementInst) != EXPECTED_POS_INST_LENGTH)
        return false;

    // find movement position
    POSITIONS posToMoveTo = (POSITIONS)atoi(movementInst);
    // retrieve long for position
    long posLong = positionToValue[posToMoveTo];
    // convert to char array
    char posArr[12];
    toCString(posArr, posLong);

    steps[0].setStep(MOVE_TO_POS, posArr);

    // set overall completion to false
    this->setOverallStepsCompleted(false);
    this->setTotalSteps(1);

    // for (int i = 0; i < this->totalSteps; i++)
    // {
    //     char movementStepsInfo[DEFAULT_CHAR_ARRAY_SIZE];
    //     sprintf(movementStepsInfo, "Step %d: %s", i, steps[i].getDetails());
    //     info(movementStepsInfo);
    // }

    return true;
};