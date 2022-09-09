#ifndef LOGGING_H
#define LOGGING_H
#include <Arduino.h>
#include <SPIFFS.h>

void Log(String str);
String ReadLog();
void DeleteLog();
String ListAllFiles();

long GetTime();
ulong GetOffset();
int GetHoursNow();
int GetMinutesNow();
int GetDayOfWeek();
void SetOffset(ulong offSetToSet);

bool GetWorkingState();
void SetWorkingState(bool on);
void SetWiFi(bool wifiConnected);

void DeleteWorkingActivity();
String ReadWorkingActivity();

#endif