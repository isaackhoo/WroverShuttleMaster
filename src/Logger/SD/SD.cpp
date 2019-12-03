#include <string.h>
#include <stdio.h>
#include <String.h>
#include "./SD.h"
#include "FS.h"
#include "SD_MMC.h"
#include "../../../src/Network/Wifi/WifiNetwork.h"
#include "../../Helper/Helper.h"

// -----------------------------
// SD PRIVATE VARIABLES
// -----------------------------
const char *LogsDirectory = "/Logs";
const char *StatusDirectory = "/Status";
const char *StatusFile = "/status.txt";

const int timestampInterval = 1000 * 60 * 5; // every 5 minutes
unsigned long lastMillis = 0;

// -----------------------------
// SD PRIVATE METHODS
// -----------------------------
bool createDir(fs::FS &fs, const char *path)
{
    if (fs.mkdir(path))
        return true;
    else
        return false;
}

bool removeDir(fs::FS &fs, const char *path)
{
    if (fs.rmdir(path))
        return true;
    else
        return false;
}

String readFile(fs::FS &fs, const char *path)
{
    String fileOutput = "";
    File file = fs.open(path);
    if (file)
    {
        while (file.available())
        {
            fileOutput += (char)file.read();
        }
        file.close();
    }

    return fileOutput;
}

bool writeFile(fs::FS &fs, const char *path, const char *message)
{
    File file = fs.open(path, FILE_WRITE);
    if (!file)
        return false;

    bool res = false;
    if (file.print(message))
    {
        file.close();
        res = true;
    }

    return res;
}

bool appendFile(fs::FS &fs, const char *path, const char *message)
{
    File file = fs.open(path, FILE_APPEND);
    if (!file)
        return false;

    bool res = false;
    if (file.print(message))
    {
        file.close();
        res = true;
    }
    return res;
}

bool renameFile(fs::FS &fs, const char *path1, const char *path2)
{
    if (fs.rename(path1, path2))
        return true;
    else
        return false;
}

bool deleteFile(fs::FS &fs, const char *path)
{
    if (fs.remove(path))
        return true;
    else
        return false;
}

// -----------------------------
// SD HELPER METHODS
// -----------------------------
char *getLatestLogFilename()
{
    // retrieves today's date and appends .txt as latest log file name
    char currentDate[11];
    getCurrentDate(currentDate);
    static char output[15];
    // strcpy_s(output, sizeof output, currentDate);
    // strcat_s(output, sizeof output, ".txt");
    strcpy(output, currentDate);
    strcat(output, ".txt");

    return output;
}

char *getLatestLogPath()
{
    char *latestLogFilename = getLatestLogFilename();
    static char pathname[21]; // /Logs/yyyy_mm_dd.txt;

    // strcpy_s(pathname, sizeof pathname, LogsDirectory);
    // strcat_s(pathname, sizeof pathname, "/");
    // strcat_s(pathname, sizeof pathname, latestLogFilename);

    strcpy(pathname, LogsDirectory);
    strcat(pathname, "/");
    strcat(pathname, latestLogFilename);

    return pathname;
}

// -----------------------------
// SD PUBLIC METHODS
// -----------------------------
bool SdInit()
{
    if (!SD_MMC.begin())
        return false;

    // sd initialized
    // initialization message
    char *initMsg = "SD Initialized";

    // create logs directory if it does not exist
    createDir(SD_MMC, LogsDirectory);
    // create a log text file if it does not exist
    logTimestampCallback(0, 0, 0);
    logToSd(initMsg);

    // TODO
    // pull from status folder and update status

    return true;
}

bool logToSd(const char *str)
{
    char endChar = str[strlen(str) - 1];

    if (!appendFile(SD_MMC, getLatestLogPath(), str))
        return false;
    if (endChar != '\n')
        if (!appendFile(SD_MMC, getLatestLogPath(), "\n"))
            return false;
    return true;
}

void logTimestampCallback(int idx, int v, int up)
{
    char timeString[9];
    getCurrentTime(timeString);
    static char timestamp[15];

    // create time stamp
    // strcpy_s(timestamp, sizeof timestamp, "[");
    // strcat_s(timestamp, sizeof timestamp, timeString);
    // strcat_s(timestamp, sizeof timestamp, "]");

    strcpy(timestamp, "[");
    strcat(timestamp, timeString);
    strcat(timestamp, "]");

    // append time stamp to sd
    logToSd(timestamp);
}

void logStatus(char *statusStr)
{
    createDir(SD_MMC, StatusDirectory);
    char statusFileName[DEFAULT_CHAR_ARRAY_SIZE];
    strcpy(statusFileName, StatusDirectory);
    strcat(statusFileName, StatusFile);

    writeFile(SD_MMC, statusFileName, statusStr);
};

char *readStatus()
{
    char statusFileName[DEFAULT_CHAR_ARRAY_SIZE];
    strcpy(statusFileName, StatusDirectory);
    strcat(statusFileName, StatusFile);

    String output = readFile(SD_MMC, statusFileName);
    static char outputCStr[DEFAULT_CHAR_ARRAY_SIZE * 2];
    strcpy(outputCStr, output.c_str());
    return outputCStr;
};

// ------------------------
// SD USAGE
// ------------------------
// listDir(SD_MMC, "/", 0);
// createDir(SD_MMC, "/mydir");
// listDir(SD_MMC, "/", 0);
// removeDir(SD_MMC, "/mydir");
// listDir(SD_MMC, "/", 2);
// writeFile(SD_MMC, "/hello.txt", "Hello ");
// appendFile(SD_MMC, "/hello.txt", "World!\n");
// readFile(SD_MMC, "/hello.txt");
// deleteFile(SD_MMC, "/foo.txt");
// renameFile(SD_MMC, "/hello.txt", "/foo.txt");
// readFile(SD_MMC, "/foo.txt");