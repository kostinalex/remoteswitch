#include "logging.h"

RTC_DATA_ATTR ulong offset = 0;

int dayOfWeek = 0;
int yearNow = 0;
int monthNow = 0;
int dayNow = 0;
int hoursNow = 0;
int minutesNow = 0;
bool wifi = false;

bool workingPINisON = false;
String delimeter = "!#";

String logsPath = "/log.txt";
String workingActivityPath = "/workingActivity.txt";

#pragma region WIFI

void SetWiFi(bool wifiConnected)
{
    wifi = wifiConnected;
}

#pragma endregion

#pragma region TIME

String GetTimeStr()
{
    long utcNowMillisec = offset + (unsigned long)(millis() / 1000);
    time_t nowTimeT = utcNowMillisec;
    struct tm *now = localtime(&nowTimeT);

    yearNow = now->tm_year;
    monthNow = now->tm_mon;
    dayNow = now->tm_mday;

    monthNow += 1;
    yearNow += 1900;

    dayOfWeek = now->tm_wday;
    hoursNow = now->tm_hour;
    minutesNow = now->tm_min;
    int secondsNow = now->tm_sec;

    if (dayOfWeek == 0)
    {
        dayOfWeek = 7;
    }
    // return "(" + String(yearNow) + "-" + String(monthNow) + "-" + String(dayNow) + " " + String(hoursNow) + ":" + String(minutesNow) + ":" + String(secondsNow) + " " + String(dayOfWeek) + " " + String(offset) +
    //        (workingPINisON ? " ON" : " OFF") + ")";
    return String(GetTime()) + "-" + (wifi ? "1" : "0") + "-" +
           (workingPINisON ? "1" : "0") + ")";
}

long GetTime()
{
    return offset + (unsigned long)(millis() / 1000);
}

ulong GetOffset()
{
    return offset;
}

int GetHoursNow()
{
    return hoursNow;
}

int GetMinutesNow()
{
    return minutesNow;
}

int GetDayOfWeek()
{
    return dayOfWeek;
}

void SetOffset(ulong offSetToSet)
{
    offset = offSetToSet;
}

#pragma endregion

#pragma region WorkingState

bool GetWorkingState()
{
    return workingPINisON;
}

void SetWorkingState(bool on)
{
    workingPINisON = on;

    Serial.println(on);
    // String oldLog = ReadLog();

    File fileWrite = SPIFFS.open(workingActivityPath, FILE_APPEND);

    if (!fileWrite)
    {
        Serial.println("There was an error opening the file for writing");
        return;
    }

    String newLog = delimeter + GetTimeStr() + String(on);

    if (newLog != "")
    {
        if (!fileWrite.print(newLog))
        {
            Serial.println("Log File write failed");
        }
    }

    fileWrite.close();
}

#pragma endregion

#pragma region LOGS

String ReadLog()
{
    String result = "";

    File file2 = SPIFFS.open(logsPath, "r");

    if (!file2)
    {
        Serial.println("Failed to open file for reading");
        return result;
    }

    while (file2.available())
    {
        // Serial.print(char(file2.read()));
        result += char(file2.read());
    }

    file2.close();

    return result;
}

void DeleteLog()
{
    SPIFFS.remove(logsPath);
}

void Log(String str)
{
    if (str == "")
    {
        return;
    }
    Serial.println(str);

    File file = SPIFFS.open(logsPath);

    if (!file)
    {
        Serial.println("There was an error opening the file for writing");
        return;
    }

    int maxLength = 4000;

    if (file.size() < maxLength)
    {
        file.close();
        File fileWrite = SPIFFS.open(logsPath, FILE_APPEND);
        String newLog = delimeter + GetTimeStr() + str;
        if (newLog != "")
        {
            if (!fileWrite.print(newLog))
            {
                Serial.println("Log File write failed");
            }
        }
        fileWrite.close();
    }
    else
    {
        file.close();
        delay(100);
        String oldLog = ReadLog();
        String newLog = oldLog + delimeter + GetTimeStr() + str;
        newLog = newLog.substring(newLog.length() - maxLength, newLog.length());
        DeleteLog();

        File fileWrite2 = SPIFFS.open(logsPath, FILE_WRITE);
        if (!fileWrite2.print(newLog))
        {
            Serial.println("Log File write failed");
        }
        fileWrite2.close();
    }
};

String ListAllFiles()
{
    String result = "";
    File root = SPIFFS.open("/");

    File file = root.openNextFile();

    while (file)
    {
        result += "!#FILE: " + String(file.name()) + " SIZE: " + String(file.size());
        // Serial.print("FILE: ");
        // Serial.print(file.name());
        // Serial.print(" SIZE: ");
        // Serial.println(file.size());
        file = root.openNextFile();
    }

    result += "!#tsize=" + String(SPIFFS.totalBytes()) + " USpace=" + String(SPIFFS.usedBytes()) + " FSpace=" + String(SPIFFS.totalBytes() - SPIFFS.usedBytes()) + " Flash=" + String(ESP.getFlashChipSize()) + delimeter;

    // Serial.print("Done reading files. SPIFFS total size=");
    // Serial.print(SPIFFS.totalBytes());
    // Serial.print(" UsedSpace=");
    // Serial.print(SPIFFS.usedBytes());
    // Serial.print(" FreeSpace=");
    // Serial.print(SPIFFS.totalBytes() - SPIFFS.usedBytes());
    // Serial.print(" Flash=");
    // Serial.println(ESP.getFlashChipSize());

    // Serial.println();

    return result;
}

#pragma endregion

#pragma region WorkingActivity

String ReadWorkingActivity()
{
    String result = "";

    File file2 = SPIFFS.open(workingActivityPath, "r");

    if (!file2)
    {
        Serial.println("Failed to open file for reading");
        return result;
    }

    while (file2.available())
    {
        // Serial.print(char(file2.read()));
        result += char(file2.read());
    }

    file2.close();

    return result;
}

void DeleteWorkingActivity()
{
    SPIFFS.remove(workingActivityPath);
}

#pragma endregion