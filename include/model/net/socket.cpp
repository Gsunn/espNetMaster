class WebSocket
{

public:
  String server;
  String port;
  String uri;
  bool connected = false; 
  bool status = false; //ON / OFF aplicar configuracion

  void readPreferences()
  {
    server = getPreferenceByName("WS", "");
    port = getPreferenceByName("WS_PORT", "");
    uri = getPreferenceByName("WS_URI", "");
    status = getPreferenceByName("WS_STATUS", "");
  }

  void showPreferences()
  {
    Serial.println("");
    _APP_DEBUG_VALUE_(F("SOCKET"), F("Server:"), server);
    _APP_DEBUG_VALUE_(F("SOCKET"), F("Port:"), port);
    _APP_DEBUG_VALUE_(F("SOCKET"), F("Uri:"), uri);
  }
};