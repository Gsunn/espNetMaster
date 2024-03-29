#include <Preferences.h>

Preferences preferences;

String getPreferenceByName(String name, String defaultValue)
{
	preferences.begin("config", false); //readOnly = false
	String pref = preferences.getString(name.c_str(), defaultValue);
	preferences.end();
	return pref;
} //getPreferenceByName

char getPreferenceBytes(String name)
{
	int schLen = preferences.getBytesLength(name.c_str());
	char buffer[schLen]; // prepare a buffer for the data
	preferences.getBytes(name.c_str(), buffer, schLen);
	return *buffer;

} //getPreferenceByName

void setPreferenceByName(String name, String value)
{
	preferences.begin("config", false); //readOnly = false
	preferences.putString(name.c_str(), value.c_str());
	preferences.end();
} //setPreferenceByName

void cleanPreference(String name)
{
	preferences.begin("config", false); //readOnly = false
	preferences.clear();
	preferences.end();
} //cleanPreference
