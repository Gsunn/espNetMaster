class Mqtt
{

public:
  String server;
  String port;
  String user;
  String pass;
  bool status = false;

  void readPreferences()
  {
    server = getPreferenceByName("MQTT_SERV", "");
    port = getPreferenceByName("MQTT_PORT", "");
    user = getPreferenceByName("MQTT_USER", "");
    pass = getPreferenceByName("MQTT_PASS", "");
    status = getPreferenceByName("MQTT_STATUS", "");
  }

  void showPreferences()
  {
    _APP_DEBUG_VALUE_("MQTT", "Server: ", server);
    _APP_DEBUG_VALUE_("MQTT", "Port: ", port);
    _APP_DEBUG_VALUE_("MQTT", "User: ", user);
    _APP_DEBUG_VALUE_("MQTT", "Pass: ", pass);
    _APP_DEBUG_VALUE_("MQTT", "Status: ", status);
    Serial.println("");
  }
};
