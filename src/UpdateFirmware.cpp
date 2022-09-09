#include <UpdateFirmware.h>

void UpdateFirmware(String url)
{
    t_httpUpdate_return updateReturn = ESPhttpUpdate.update(url);

    switch (updateReturn)
    {
    case HTTP_UPDATE_FAILED:
        Serial.println("Update failed");
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("No updates");
        break;

    case HTTP_UPDATE_OK:
        Serial.println("Ok");
        break;
    }
}
