#include <ESPAsyncWebServer.h>
// #include <ArduinoJson.h>

AsyncWebServer server(80);

// Prototype

// void initWebServer();
// void loadDOM();
// void loadFunctions();
// void loadConfigForms();

String processor(const String &var)
{
  _APP_DEBUG_("PROCESSOR", var);

  if (var == "AP_SSID" && AP.ssid != "")
  {
    return AP.ssid;
  }
  else if (var == "SSID_ESP_NOW" && NOW.ssid != "")
  {
    return NOW.ssid;
  }
  else if (var == "CHANNEL" && AP.chan != "")
  {
    return AP.chan;
  }
  else if (var == "WS_STATUS")
  {
    return SOCKET.status ? "on" : "off";
  }
  else if (var == "WEBSOCKET" && SOCKET.server != "")
  {
    return SOCKET.server;
  }
  else if (var == "WS_URI" && SOCKET.uri != "")
  {
    return SOCKET.uri;
  }
  else if (var == "WS_PORT" && SOCKET.port != "")
  {
    return SOCKET.port;
  }
  else if (var == "MQTT_SERVER" && MQTT.server != "")
  {
    return MQTT.server;
  }
  else if (var == "MQTT_PORT" && MQTT.port != "")
  {
    return MQTT.port;
  }
  else if (var == "MQTT_USER" && MQTT.user != "")
  {
    return MQTT.user;
  }
  else if (var == "MQTT_STATUS")
  {
    return MQTT.status ? "on" : "off";
  }

  return String();
}

/**
 *  Load Index & CSS & JS & Bootstrap
 */
void loadDOM()
{

  // Root /
  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request) {
    _APP_DEBUG_("SETUP", "Init endpoint");
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/css/unity.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/unity.css", "text/css");
  });

  // Bootstrap
  // server.on("/css/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/css/bootstrap.min.css", "text/css");
  // });

  // server.on("/css/bootstrap-select.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/css/bootstrap-select.min.css", "text/css");
  // });

  // server.on("/css/bootstrap4-toggle.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/css/bootstrap4-toggle.min.css", "text/css");
  // });

  // Style
  server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/style.css", "text/css");
  });

  // Js

  server.on("/js/unity.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/unity.js", "text/js");
  });

  //ICON & FONTS
  server.on("/fonts/icomoon.eot", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/fonts/icomoon.eot", "font/eot");
  });

  server.on("/fonts/icomoon.ttf", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/fonts/icomoon.ttf", "font/ttf");
  });

  server.on("/fonts/icomoon.woff", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/fonts/icomoon.ttf", "font/woff");
  });

  server.on("/fonts/icomoon.woff", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/fonts/icomoon.svg", "image/svg+xml");
  });

  // server.on("/js/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/js/jquery.min.js", "text/js");
  // });

  // Popper.js
  // server.on("/js/popper.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/js/popper.min.js", "text/js");
  // });

  // server.on("/js/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/js/bootstrap.min.js", "text/js");
  // });

  // server.on("/js/bootstrap-select.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/js/bootstrap-select.min.js", "text/js");
  // });

  // server.on("/js/bootstrap4-toggle.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/js/bootstrap4-toggle.min.js", "text/js");
  // });
}


void setData(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{

   // Aquí va el código para administrar la solicitud de publicación
  // Los datos se reciben en la variable 'data'
  // Analizar datos
 
  char buffer[len];
  memcpy(&buffer, data, len + 1);

  StaticJsonDocument<250> doc;  // create a buffer that fits for you
  deserializeJson(doc, buffer); //Parse message

  String mac = doc["mac"];
  int gpio = doc["gpio"];
  bool value = doc["value"];
  Serial.println(mac);
  Serial.println(gpio);
  Serial.println(value);

  String json;
  serializeJson(doc, json);

  sendMsgToStation(json);

  request->send(200, "text/plain", "OK");
}

/**
 *  Load Functions Urls
 */
void loadFunctions()
{
  // Get Wifi List
  server.on("/apList", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", scanNetworks().c_str());
  });

  // Get station ESPNOW list
  server.on("/topology", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", topology().c_str());
  });

  // Esp Restart
  server.on("/espRestart", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", "ESP.restart()");
    ESP.restart();
  });

  server.on("/deviceStatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", deviceStatus().c_str());
  });

  server.on("/gpioAction", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, setData);

}

/**
 *  Load Network & WebSockets Forms
 */
void loadConfigForms()
{
  // Network Setup
  server.on("/getFormSetup", HTTP_POST, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;

    if (request->hasParam("apList", true))
    {
      setPreferenceByName("SSID_AP", request->getParam("apList", true)->value());
      inputMessage = getPreferenceByName("SSID_AP", "");
      inputParam = "SSID_AP";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("apPass", true))
    {
      setPreferenceByName("PASS_AP", request->getParam("apPass", true)->value());
      inputMessage = getPreferenceByName("PASS_AP", "");
      inputParam = "PASS_AP";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("channel", true))
    {
      setPreferenceByName("CHAN_AP", request->getParam("channel", true)->value());
      inputMessage = getPreferenceByName("CHAN_AP", "");
      inputParam = "CHAN_AP";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("ssid", true))
    {
      setPreferenceByName("SSID_AP_ESPNOW", request->getParam("ssid", true)->value());
      inputMessage = getPreferenceByName("SSID_AP_ESPNOW", "");
      inputParam = "SSID_AP_ESPNOW";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("pwd", true))
    {
      setPreferenceByName("PASS_AP_ESPNOW", request->getParam("pwd", true)->value());
      inputMessage = getPreferenceByName("PASS_AP_ESPNOW", "");
      inputParam = "PASS_AP_ESPNOW";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    //Redirige al index
    request->send(SPIFFS, "/index.html");
  });

  // WebSocket & Mqtt Setup
  server.on("/getFormComunications", HTTP_POST, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;

    // WS
    if (request->hasParam("wsSwitch", true))
    {
      setPreferenceByName("WS_STATUS", request->getParam("wsSwitch", true)->value());
      inputMessage = getPreferenceByName("WS_STATUS", "");
      inputParam = "WS";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("websocket", true))
    {
      setPreferenceByName("WS", request->getParam("websocket", true)->value());
      inputMessage = getPreferenceByName("WS", "");
      inputParam = "WS";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("port", true))
    {
      setPreferenceByName("WS_PORT", request->getParam("port", true)->value());
      inputMessage = getPreferenceByName("WS_PORT", "");
      inputParam = "WS_PORT";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("uri", true))
    {
      setPreferenceByName("WS_URI", request->getParam("uri", true)->value());
      inputMessage = getPreferenceByName("WS_URI", "");
      inputParam = "WS_URI";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    // MQTT
    if (request->hasParam("mqttSwitch", true))
    {
      setPreferenceByName("MQTT_STATUS", request->getParam("mqttSwitch", true)->value());
      inputMessage = getPreferenceByName("MQTT_STATUS", "");
      inputParam = "MQTT_STATUS";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("mqtt_server", true))
    {
      setPreferenceByName("MQTT_SERV", request->getParam("mqtt_server", true)->value());
      inputMessage = getPreferenceByName("MQTT_SERV", "");
      inputParam = "MQTT_SERV";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("mqtt_server_port", true))
    {
      setPreferenceByName("MQTT_PORT", request->getParam("mqtt_server_port", true)->value());
      inputMessage = getPreferenceByName("MQTT_PORT", "");
      inputParam = "MQTT_PORT";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("mqtt_username", true))
    {
      setPreferenceByName("MQTT_USER", request->getParam("mqtt_username", true)->value());
      inputMessage = getPreferenceByName("MQTT_USER", "");
      inputParam = "MQTT_USER";
      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    if (request->hasParam("mqtt_password", true))
    {
      setPreferenceByName("MQTT_PASS", request->getParam("mqtt_password", true)->value());
      inputMessage = getPreferenceByName("MQTT_PASS", "");
      inputParam = "MQTT_PASS";

      _APP_DEBUG_("FORM", inputParam + " " + inputMessage);
    }

    //Redirige al index
    request->send(SPIFFS, "/index.html");
  });
}

/**
 *  Initialize the Web Server
 */
void initWebServer()
{
  _APP_DEBUG_("WEB", "Starting the Web Server");

  loadDOM();

  loadFunctions();

  loadConfigForms();

  server.begin();

  _APP_DEBUG_("WEB", "Started the Web Server");

} //initWebServer
