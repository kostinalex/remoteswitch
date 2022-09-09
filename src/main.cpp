#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <logging.h>
#include <DAO.h>
#include <UpdateFirmware.h>
#include <misc.h>

int actionPin = 4;

ulong lastTimeConnectedToWiFi = 0;
uint8_t timeToTryToConnectToWiFiSec = 30;
ulong lastTimeWeTriedToConnectAndFailed = 0;
String firmwareVersion = "TransamTest.0.0.18";
String domainName = "https://transamapp.transamcarriers.com";
int callAPIviaWiFiEverySec = 1;

bool home = true;
String name = "HomeTest3";

#pragma region TEST

void TestSetPrograms()
{
  Program programsToWrite[10];

  for (int i = 0; i < 10; i++)
  {
    Program newP;
    newP.StartMinutes = 400 + i;
    newP.DurationMinutes = i;
    newP.DaysOfWeek = "0101011";
    newP.Periodicity = 2;
    programsToWrite[i] = newP;
  }

  SetPrograms(programsToWrite);
}

void TestSetCredentials()
{
  Credentials credentials;

  credentials.Ssid = "";
  credentials.Pass = "";

  if (home)
  {
    credentials.Ssid = "Kostin WiFi Family";
    credentials.Pass = "";
  }

  SetCredentials(credentials);
}

#pragma endregion

void ConnectToWifiWithCredentials(Credentials credentialsToRead[])
{
  char ssid[64];
  char pass[64];
  credentialsToRead[0].Ssid.toCharArray(ssid, credentialsToRead[0].Ssid.length() + 1);
  credentialsToRead[0].Pass.toCharArray(pass, credentialsToRead[0].Pass.length() + 1);

  Log("SSID:" + String(ssid) + " PASS:" + String(pass));
  WiFi.begin(ssid, pass);

  int startTimeForConnection = millis();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
    if ((millis() - startTimeForConnection) > timeToTryToConnectToWiFiSec * 1000)
    {
      Log("No wifi");
      lastTimeWeTriedToConnectAndFailed = millis();
      return;
    }
  }
  Serial.println("Connected IP: " + String(WiFi.localIP()));
  delay(50);
}

void ConnectToWiFi()
{
  Credentials credentialsToRead[1];
  String creds = GetCredentials(credentialsToRead);
  ConnectToWifiWithCredentials(credentialsToRead);
}

void HttpReportActivity(bool on)
{
  HTTPClient http;
  http.begin(domainName + "/api/reportworkingactivity/" + GetMacAddress() + "/" + String(GetTime()) + "/" + (on ? "true" : "false"));
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.GET();
  String payload = http.getString();

  if (httpResponseCode == 200)
  {
    Serial.println("\r\np!!!!!!!!!!!!!!!!!!!!!!!!!!!!Activity reported");
  }
}

void MakeRegularHTTPCall()
{
  Serial.println("Making regular http call");
  String mac = GetMacAddress();
  Serial.println("Mac:" + mac + " firmwareVersion:" + firmwareVersion);

  String logsToSend = ReadLog();
  // Serial.println("\r\nLogs:\r\n" + logsToSend + "\r\n\r\n");

  String requestBody = "{\"deviceId\":\"" + name + "\",\"firmwareVersion\":\"" + firmwareVersion + "\"" + ",\"logs\":\"" + logsToSend + "\"" + "}";
  Serial.println("\r\nrequestBody:\r\n" + requestBody);

  HTTPClient http;
  http.begin(domainName + "/api/remoteswitch");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(requestBody);
  String payload = http.getString();

  Serial.println("\r\npayload=" + payload);

  if (httpResponseCode == 200)
  {
    DeleteLog();

    String timeNowStr = GetProperty(payload, "timenow");
    char larray[timeNowStr.length() + 1];
    timeNowStr.toCharArray(larray, sizeof(larray));
    SetOffset(atol(larray) - (unsigned long)(millis() / 1000));

    String updateUrl = GetProperty(payload, "updateUrl");

    if (updateUrl != "null")
    {
      Log("updateUrl=" + updateUrl);
      UpdateFirmware(updateUrl);
    }
    else
    {
      Serial.println("no upd");
    }

    String workStr = GetProperty(payload, "work");
    Serial.println("workStr=" + workStr);

    if (workStr == "ON")
    {
      digitalWrite(actionPin, HIGH);
    }
    else
    {
      digitalWrite(actionPin, LOW);
    }
  }
  else
  {
    Log("Http: " + String(httpResponseCode));
  }

  http.end();
}

void setup()
{
  Serial.begin(9600);
  delay(200);
  pinMode(actionPin, OUTPUT);

  Serial.println("Working...");
  Serial.println("Working...");
  Serial.println("Working...");
  Serial.println("Firmware version=" + firmwareVersion);

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  DeleteLog();

  // DeletePrograms();
  // TestSetPrograms();

  DeleteCredentials();
  TestSetCredentials();

  Log("filesStats: " + ListAllFiles());

  Log("Check SSID and pass");
  Credentials credentialsToRead[1];
  String creds = GetCredentials(credentialsToRead);
  Log("SSID:" + credentialsToRead[0].Ssid + " PASS:" + credentialsToRead[0].Pass);

  if (credentialsToRead[0].Ssid == "")
  {
    Log("NO creds for wifi");

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
  }
  else
  {
    Log("Connecting to wifi:");
    ConnectToWifiWithCredentials(credentialsToRead);
  }
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    SetWiFi(true);
    lastTimeConnectedToWiFi = millis();
    MakeRegularHTTPCall();
  }
  else
  {
    SetWiFi(false);
    // try to reconnect or initialize bluetooth

    long numberOfSecondsWiFiWasInactive = (millis() - lastTimeConnectedToWiFi) / 1000;
    Serial.println("Disconnected for last " + String(numberOfSecondsWiFiWasInactive) + " sec");

    if (numberOfSecondsWiFiWasInactive < 5 * 60)
    {
      Serial.println("Try reconnect in 2");
      if (millis() - lastTimeWeTriedToConnectAndFailed > 2 * 60 * 1000)
      {
        ConnectToWiFi();
      }
    }
    else
    {
      Serial.println("Try reconnect in 5");
      if (millis() - lastTimeWeTriedToConnectAndFailed > 5 * 60 * 1000)
      {
        ConnectToWiFi();
      }
    }
  }
}