#include "SD.h"
#include "SD_MMC.h"
#include "Network.h"
#include "Helper.h"

// -----------------------------
// SD PRIVATE VARIABLES
// -----------------------------

const char *LogsDirectory = "/Logs";

// -----------------------------
// SD METHODS
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
            fileOutput += file.read();
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
    char *date = getCurrentDate();
    static char latestLogFilename[15]; // date.txt
    strcpy(latestLogFilename, date);
    strcat(latestLogFilename, ".txt");

    return latestLogFilename;
}

char *getLatestLogPath()
{
    char *latestLogFilename = getLatestLogFilename();
    static char pathname[21]; // /Logs/yyyy_mm_dd.txt;

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
    char *latestLogFilename = getLatestLogFilename();
    if (!readFile(SD_MMC, latestLogFilename))
        writeFile(SD_MMC, getLatestLogPath(), initMsg);
    else
        logToSd("Sd initialized");

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

bool logTimestampToSd()
{
    char *time = getCurrentTime();

    static char timestamp[15];

    // create time stamp
    strcpy(timestamp, "[");
    strcat(timestamp, time);
    strcat(timestamp, "]");

    logToSd(timestamp);
}

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