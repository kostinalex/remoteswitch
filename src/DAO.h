#ifndef DAO_H
#define DAO_H
#include <Arduino.h>
#include <SPIFFS.h>

class Credentials
{
public:
    String Ssid;
    String Pass;
};

class Program
{
public:
    int StartMinutes;
    int DurationMinutes;
    String DaysOfWeek;
    byte Periodicity;
};

void SetPrograms(Program programs[]);
void PrintPrograms(Program programs[]);
void DeletePrograms();
bool AreTherePrograms(Program programs[]);
String GetPrograms(Program programs[]);
String ReadProgramsStr();
void WriteProgramsStr(String str);

void SetCredentials(Credentials credentials);
String GetCredentials(Credentials credentials[]);
void DeleteCredentials();

#endif