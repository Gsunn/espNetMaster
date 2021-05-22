#include <SPIFFS.h>

unsigned int getFreeSpiMemory();

/**
 * Mount the Flash
 */
void mountFlash()
{
  _APP_DEBUG_("FLASH", !SPIFFS.begin(true) ? "Error mounting" : "Mounted!");
}

/**
 * Show the Files 
 */
void openFiles()
{

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file)
  {
    _APP_DEBUG_VALUE_("FLASH", "File:", file.name());
    file = root.openNextFile();
  }
}

/**
 * Show the Memory State 
 */
void showMemory()
{
  _APP_DEBUG_VALUE_("FLASH", "Total Bytes: ", SPIFFS.totalBytes());
  _APP_DEBUG_VALUE_("FLASH", "Used Bytes: ", SPIFFS.usedBytes());
}

unsigned int getFreeSpiMemory()
{
  unsigned int res = SPIFFS.totalBytes() - SPIFFS.usedBytes();
  _APP_DEBUG_VALUE_("SPI FREE","", res);
  return res;
}

/**
 * Show the Memory State 
 */
void cleanApPreferences()
{
 
  cleanPreference("SSID_AP");
  cleanPreference("PASS_AP");
  cleanPreference("CHAN_AP");
  _APP_DEBUG_("---------------------------", "CLEAR");
} //cleanWifiPreferences
