#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <model/net/mqtt.cpp>

Mqtt MQTT;
WiFiClient espClient;
PubSubClient mqtt_client(espClient);


bool mqttStatus(){
  return MQTT.status;
}

/**
 *  Mqtt Callback Event
 */
void mqttCallback(char *topic, byte *message, unsigned int length)
{
  String messageTemp;
  _APP_DEBUG_VALUE_("MQTT", "Message arrived on topic: ", topic);
  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)message[i];
  }
  _APP_DEBUG_VALUE_("MQTT", ". Message: ", messageTemp);
}

/**
 *  Mqtt Client Reconcet 
 */
void mqttClientReconect(PubSubClient &_mqtt_client)
{
  while (!_mqtt_client.connected())
  {
    _APP_DEBUG_("MQTT", "Attempting MQTT connection...");

    if (_mqtt_client.connect("ESP32Client", MQTT.user.c_str(), MQTT.pass.c_str()))
    {
      _APP_DEBUG_("MQTT", "connected MQTT");
      _mqtt_client.subscribe("esp");
      _APP_DEBUG_("MQTT", "Subscribe to esp");
    }
    else
    {
      _APP_DEBUG_VALUE_("MQTT", "Client Reconet Failed, rc=", _mqtt_client.state());
      _APP_DEBUG_("MQTT", "Please Wait - Trying again in 5 seconds");

      delay(5000);
    }
  }
}

/**
 *  Mqtt Publish 
 */
void mqttPublish(String data)
{
  if (!mqtt_client.connected())
  {
    _APP_DEBUG_("MQTT", "No connected try reconnect.");
    mqttClientReconect(mqtt_client);
  }
  mqtt_client.publish("esp", data.c_str());
  _APP_DEBUG_VALUE_("MQTT", "Publish on Topic ESP", data.c_str());
}

void initMqtt()
{
  if (MQTT.server != "" && MQTT.port != "" && MQTT.user != "" && MQTT.pass != "")
  {

    _APP_DEBUG_("MQTT", "Starting...");
    _APP_DEBUG_VALUE_("MQTT", "Server:", MQTT.server);
    _APP_DEBUG_VALUE_("MQTT", "Port:", MQTT.port);
    _APP_DEBUG_VALUE_("MQTT", "User:", MQTT.user);
    _APP_DEBUG_VALUE_("MQTT", "Password:", MQTT.pass);

    mqtt_client.setServer(MQTT.server.c_str(), MQTT.port.toInt());
    mqtt_client.setCallback(mqttCallback);

    MQTT.status = mqtt_client.connect("ESP32Client", MQTT.user.c_str(), MQTT.pass.c_str());

    _APP_DEBUG_VALUE_("MQTT", "Conection: ", MQTT.status ? "On" : "Fail");
  }
} //initMqtt
