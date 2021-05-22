class Now
{

public:
  String ssid;
  String pass;
  bool status = false;
  String response;

  void readPreferences()
  {
    ssid = getPreferenceByName("SSID_AP_ESPNOW", "");
    pass = getPreferenceByName("PASS_AP_ESPNOW", "");
  }

  void showPreferences()
  {
    _APP_DEBUG_VALUE_("NOW PREFERENCES", "Ssid: ", ssid);
    _APP_DEBUG_VALUE_("NOW PREFERENCES", "Pass: ", pass);
    Serial.println("");
  }
};