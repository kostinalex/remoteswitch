#include "dao.h"

// #include "logging.h"

char *programsPath = "/programs.json";
char *credentialsPath = "/credentials.json";
String separator = "!#";

#pragma region READWRITE

void WriteToFile(String str, char *filePath)
{
    File fileWrite = SPIFFS.open(filePath, "w");
    if (!fileWrite)
    {
        Serial.println("There was an error opening the file for writing");
        return;
    }

    if (!fileWrite.print(str))
    {
        Serial.println("File write failed" + String(filePath));
    }

    fileWrite.close();
}

String ReadFile(char *filePath)
{
    String result = "";
    File file2 = SPIFFS.open(filePath, "r");

    if (!file2)
    {
        Serial.println("Failed to open file for reading");
        return result;
    }

    while (file2.available())
    {
        result += char(file2.read());
    }

    file2.close();
    return result;
}

String ReadProgramsStr()
{
    String result = "";
    File file2 = SPIFFS.open(programsPath, "r");

    if (!file2)
    {
        Serial.println("Failed to open file for reading");
        return result;
    }

    while (file2.available())
    {
        result += char(file2.read());
    }

    file2.close();
    return result;
}

void WriteProgramsStr(String str)
{
    File fileWrite = SPIFFS.open(programsPath, "w");
    if (!fileWrite)
    {
        Serial.println("There was an error opening the file for writing");
        return;
    }

    if (!fileWrite.print(str))
    {
        Serial.println("File write failed" + String(programsPath));
    }

    fileWrite.close();
}

#pragma endregion

#pragma region CREDENTIALS

void SetCredentials(Credentials credentials)
{
    String creds = credentials.Ssid + "!!##!!$$" + credentials.Pass;
    WriteToFile(creds, credentialsPath);
    // Serial.println("Credentials set....");
}

String GetCredentials(Credentials credentials[])
{
    String delimiter="!!##!!$$";
    String result = ReadFile(credentialsPath);

    if (result != "" && result.indexOf(delimiter) > -1)
    {
        credentials[0].Ssid = result.substring(0, result.indexOf(delimiter));
        credentials[0].Pass = result.substring(result.indexOf(delimiter) + delimiter.length());
    }
    return result;
}

void DeleteCredentials()
{
    SPIFFS.remove(credentialsPath);
}

#pragma endregion

#pragma region PROGRAMS

void SetPrograms(Program programs[])
{
    String programsStr = "";
    for (int i = 0; i < 10; i++)
    {
        if (programs[i].DurationMinutes > 0)
        {
            programsStr = programsStr + String(programs[i].StartMinutes) + "," + String(programs[i].DurationMinutes) + "," + String(programs[i].DaysOfWeek) + "," + String(programs[i].Periodicity) + separator;
        }
    }

    WriteToFile(programsStr, programsPath);
}

String GetPrograms(Program programs[])
{
    String result = ReadFile(programsPath);
    // Serial.println("\r\nGetPrograms:\r\n" + result);
    String leftStr = result;

    if (result != "")
    {
        for (int i = 0; i < 10; i++)
        {
            Program newProgram;
            String program = leftStr.substring(0, leftStr.indexOf(separator));

            for (int j = 0; j < 3; j++)
            {
                if (j == 0)
                {
                    newProgram.StartMinutes = program.substring(0, program.indexOf(",")).toInt();
                }

                if (j == 1)
                {
                    newProgram.DurationMinutes = program.substring(0, program.indexOf(",")).toInt();
                }

                if (j == 2)
                {
                    newProgram.DaysOfWeek = program.substring(0, program.indexOf(","));
                    newProgram.Periodicity = program.substring(program.indexOf(",") + 1).toInt();
                }

                program = program.substring(program.indexOf(",") + 1);
            }

            programs[i] = newProgram;

            leftStr = leftStr.substring(leftStr.indexOf(separator) + separator.length());
        }
    }
    return result;
}

void DeletePrograms()
{
    SPIFFS.remove(programsPath);
}

bool AreTherePrograms(Program programsToRead[])
{

    for (int i = 0; i < 10; i++)
    {
        if (programsToRead[i].DurationMinutes > 0 && programsToRead[i].DurationMinutes <= 1440 && programsToRead[i].StartMinutes >= 0 && programsToRead[i].StartMinutes <= 1440)
        {
            return true;
        }
    }
    return false;
}

void PrintPrograms(Program programsToRead[])
{
    for (int i = 0; i < 10; i++)
    {
        Serial.println(
            " StartMinutes=" + String(programsToRead[i].StartMinutes) +
            " DurationMinutes=" + String(programsToRead[i].DurationMinutes) +
            " DaysOfWeek=" + String(programsToRead[i].DaysOfWeek) +
            " Periodicity=" + String(programsToRead[i].Periodicity));
    }
}

#pragma endregion


