#include <misc.h>

String GetMacAddress()
{
  uint64_t chipid;
  chipid = ESP.getEfuseMac();

  unsigned long long1 = (unsigned long)((chipid & 0xFFFF0000) >> 16);
  unsigned long long2 = (unsigned long)((chipid & 0x0000FFFF));

  String hex = String(long1, HEX) + String(long2, HEX);

  return hex;
}


String GetProperty(String payload, String propertyName)
{
  return payload.substring(payload.indexOf("<" + propertyName + ">") + String("<" + propertyName + ">").length(), payload.indexOf("</" + propertyName + ">"));
}

