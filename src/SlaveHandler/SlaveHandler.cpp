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
void SlaveHandler::setTotalSteps(int total)
{
    this->totalSteps = total;
};

void SlaveHandler::incCurrentStepIndex()
{
    this->currentStepIndex++;
};

void SlaveHandler::resetCurrentStepIndex()
{
    this->currentStepIndex = 0;
};

void SlaveHandler::setOverallStepsCompleted(bool isComplete)
{
    this->overallStepsCompleted = isComplete;
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
    //  01   03   05   07   09  11
    //
    //  00   02   04   06   08  10

    int binCol = bin / 2; //  0 / 2 = 0; 3 / 2 = 1; etc..

    pos += MOTORCOUNT_BUFFER_HOLE_TO_CENTER_OF_PILLAR;                                                              // for buffer
    pos += ((col - 1) * MOTOTCOUNT_PER_COLUMN);                                                                     // for full columns
    pos += (MOTORCOUNT_CENTER_OF_PILLAR_TO_ADAJ_SLOTHOLE_CENTER + ((bin / 2) * MOTORCOUNT_BTWN_SLOTS_WITHIN_RAIL)); // for in between bin slots

    /*pos += A;                              // for const buffer area to first pillar
    pos += (col - 1) * D;                  // for whole columns
    pos += B + ((((bin + 1) / 2) - 1) * C) // for bin in col

    // pos += GAP_BETWEEN_COLUMN_AND_BIN; // for first bin
    // pos += binCol * (GAP_BETWEEN_BIN_WIDTH + BIN_WIDTH);
    // pos += BIN_WIDTH / 2; // brings shuttle to center of bin column
    */

    char posStr[21];
    toCString(posStr, pos);
    strcpy(output, posStr);
};

int SlaveHandler::getArmExtensionDirection(char *inRack)
{
    // get direction to extend arms to
    //    Rack|     |Rack     Rack|     |Rack
    //  05  04|     |04  05 05  04|     |04  05
    //  03  02|     |02  03 03  02|     |02  03
    //  01  00|     |00  01 01  00|     |00  01
    //  --00--|     |--01-- --02--|     |--03--
    //  11  10|     |10  11 11  10|     |10  11
    //  09  08|     |08  09 09  08|     |08  09
    //  07  06|     |06  07 07  06|     |06  07
    //  05  04|     |04  05 05  04|     |04  05
    //  03  02|     |02  03 03  02|     |02  03
    //  01  00|     |00  01 01  00|     |00  01

    // get extension direction
    int rackInt = atoi(inRack);
    int direction = rackInt % 2;
    int extensionDirection = 0;
    if (direction == 0)
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
    if (binDepth == 0)
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

void SlaveHandler::SlaveHandler::handle(char *serialInput)
{
    // if shuttle is not in active status, it is receiving delayed
    // responses. ignore these.
    if (status.getState() == IDLE)
        return;

    // handle slave reply
    bool res = steps[this->currentStepIndex].evaluateStep(serialInput);

    if (!res)
    {
        // do nothing
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
        default:
            break;
        }

        // check for more steps to do
        if (this->currentStepIndex < this->totalSteps - 1)
            this->incCurrentStepIndex();
        else
            this->setOverallStepsCompleted(true);
    }

    // check for more steps to perform
    if (!this->overallStepsCompleted)
    {
        // there are more steps to do
        if (steps[this->currentStepIndex].getStepAction() != DUMMY_STEP)
        {
            this->beginNextStep();
        }
    }
    else
    {
        // job completed
        // update wcs on job completion
        wcsHandler.sendJobCompletionNotification(status.getActionEnum(), "01");
        status.setState(IDLE);
        // reset slave handler
        this->reset();
    }
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

bool SlaveHandler::send(char *slaveInst)
{
    if (!this->ss.ready())
        return false;
    if (strlen(slaveInst) <= 0)
        return false;
    return this->ss.ssWrite(slaveInst);
};

// --------------------------------
// SLAVE HANDLER PUBLIC METHODS
// --------------------------------
void SlaveHandler::init()
{
    // initialise slave serial
    this->ss.init(&Serial, "SlaveMain");
};

void SlaveHandler::run()
{
    // checks slave serial for input
    if (this->ss.ssRead())
    {
        this->handle(this->ss.getReadString());
    }
};

void SlaveHandler::beginNextStep()
{
    info("Starting next step");
    // change step status to active/pending
    steps[this->currentStepIndex].setStatus(STEP_PENDING);
    // set shuttle status to active
    status.setActiveState();
    // send instructions over slave serial
    this->send(steps[this->currentStepIndex].getSlaveString());
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

    successfullyProcessed = cutStr(storageInst, inRack, 0, RACK_ID_LENGTH);
    successfullyProcessed = cutStr(storageInst, inCol, 0, COLUMN_ID_LENGTH);
    successfullyProcessed = cutStr(storageInst, binInColPos, 0, BIN_POS_ID_LENGTH);

    if (!successfullyProcessed)
        return false;

    // clear to move arr
    char clearToMoveArr[DEFAULT_CHAR_ARRAY_SIZE];
    itoa(CLEAR_TO_MOVE, clearToMoveArr, 10);

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
            binPosState = RIGHT_BOTH_OCCUPIED;
        else if (depth == SECOND_DEPTH)
            binPosState = RIGHT_COMPLETELY_EMPTY;
    }
    GET_TWO_DIGIT_STRING(noBinInStorageSlotArr, binPosState);

    // get pushing fingers
    char pushingFingersArr[DEFAULT_CHAR_ARRAY_SIZE];
    this->getPushingFingers(extensionDirection, pushingFingersArr);

    steps[0].setStep(CHECK_CLEAR_TO_MOVE, clearToMoveArr);
    steps[1].setStep(MOVE_TO_POS, bufferPosArr);
    steps[2].setStep(CHECK_BUFFER_BIN_SLOT, binInBufferSlotArr);
    steps[3].setStep(EXTEND_ARM, storageBufferDepthArr);
    steps[4].setStep(EXTEND_FINGERS, pullingFingersArr);
    steps[5].setStep(RETRACT_ARM, armHomeArr);
    steps[6].setStep(RETRACT_FINGERS, pullingFingersArr);
    steps[7].setStep(CHECK_CLEAR_TO_MOVE, clearToMoveArr);
    steps[8].setStep(MOVE_TO_BIN, binPosArr);
    steps[9].setStep(CHECK_RACK_BIN_SLOT, noBinInStorageSlotArr);
    steps[10].setStep(EXTEND_FINGERS, pushingFingersArr);
    steps[11].setStep(EXTEND_ARM, extensionResultArr);
    steps[12].setStep(RETRACT_FINGERS, pushingFingersArr);
    steps[13].setStep(RETRACT_ARM, armHomeArr);

    this->setOverallStepsCompleted(false);
    this->setTotalSteps(14);

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

    successfullyProcessed = cutStr(retrievalInst, inRack, 0, RACK_ID_LENGTH);
    successfullyProcessed = cutStr(retrievalInst, inCol, 0, COLUMN_ID_LENGTH);
    successfullyProcessed = cutStr(retrievalInst, binInColPos, 0, BIN_POS_ID_LENGTH);

    if (!successfullyProcessed)
        return false;

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

    // arm home pos to char
    char armHomeArr[DEFAULT_CHAR_ARRAY_SIZE];
    itoa(ARM_HOME_POSITION, armHomeArr, 10);

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

    // clear to move char
    char clearToMoveArr[DEFAULT_CHAR_ARRAY_SIZE];
    itoa(CLEAR_TO_MOVE, clearToMoveArr, 10);

    //steps[0].setStep(CHECK_CLEAR_TO_MOVE, clearToMoveArr);
    steps[0].setStep(MOVE_TO_BIN, binPosArr);
    steps[1].setStep(CHECK_RACK_BIN_SLOT, binInRackSlotArr);
    steps[2].setStep(EXTEND_ARM, extensionResultArr);
    steps[3].setStep(EXTEND_FINGERS, pullingFingersId);
    steps[4].setStep(RETRACT_ARM, armHomeArr);
    steps[5].setStep(RETRACT_FINGERS, pullingFingersId);
    //steps[7].setStep(CHECK_CLEAR_TO_MOVE, clearToMoveArr);
    steps[6].setStep(MOVE_TO_POS, bufferPosArr);
    steps[7].setStep(CHECK_BUFFER_BIN_SLOT, noBinInRetrievalBufferArr);
    steps[8].setStep(EXTEND_FINGERS, pushingFingersArr);
    steps[9].setStep(EXTEND_ARM, retrievalBufferArr);
    steps[10].setStep(RETRACT_ARM, armHomeArr);
    steps[11].setStep(RETRACT_FINGERS, pushingFingersArr);

    // set overall completion to false
    this->setOverallStepsCompleted(false);
    this->setTotalSteps(12);

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

    return true;
};