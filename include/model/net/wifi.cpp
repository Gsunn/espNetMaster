class WifiPoint
{

public:
  String ssid;
  String mac;
  String pass;
  String chan;

  void readPreferences()
  {
    ssid = getPreferenceByName("SSID_AP", "");
    pass = getPreferenceByName("PASS_AP", "");
    chan = getPreferenceByName("CHAN_AP", "");
  }

  void showPreferences()
  {
    _APP_DEBUG_VALUE_("AP PREFERENCES", "Ssid: ", ssid);
    _APP_DEBUG_VALUE_("AP PREFERENCES", "Pass: ", pass);
    _APP_DEBUG_VALUE_("AP PREFERENCES", "Chan: ", chan);
    Serial.println("");
  }

  void show()
  {
    Serial.print(ssid);
    Serial.print(" [");
    Serial.print(mac);
    Serial.print("]");
    Serial.print(" Ch ");
    Serial.print(chan);
    Serial.println("");
  }
};
