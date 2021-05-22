#include <WebSocketsClient.h>
#include <model/net/socket.cpp>

WebSocket SOCKET;
WebSocketsClient webSocketClient;

/**
    Hex dump
*/
void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
{
  const uint8_t *src = (const uint8_t *)mem;
  Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for (uint32_t i = 0; i < len; i++)
  {
    if (i % cols == 0)
    {
      Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    Serial.printf("%02X ", *src);
    src++;
  }
  Serial.printf("\n");
}

/**
    Web Socket Event Listener
*/
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  #ifdef _APP_DEBUG_ON
    Serial.println("");
  #endif
  switch (type)
  {
  case WStype_DISCONNECTED:
    SOCKET.connected = false;
    _APP_DEBUG_(F("SOCKET"), F("Disconnected!"));
    break;
  case WStype_CONNECTED:
    #ifdef _APP_DEBUG_ON
      Serial.printf("[SOCKET] Connected to url: %s\n", payload);
    #endif
    // send message to server when Connected
    SOCKET.connected = true;
    webSocketClient.sendTXT("{\"Connected\":\"" +  WiFi.macAddress() + "\"}");
    break;
  case WStype_TEXT:
    //Serial.printf("[SOCKET] Get text: %s\n", payload);

    char msg[250];
    snprintf(msg, sizeof(msg), "%s", payload);

    _APP_DEBUG_("[WS] MSG IN >>> ", msg);

    // Envia el meensaje a la estacion 
    sendMsgToStation(msg);

    // send message to server
    //webSocketClient.sendTXT("message here");
    break;
  case WStype_BIN:
  #ifdef _APP_DEBUG_ON
    Serial.printf(F("[SOCKET] Get binary length: %u\n"), length);
  #endif  
    hexdump(payload, length);

    // send data to server
    // webSocket.sendBIN(payload, length);
    break;
  case WStype_ERROR:
  case WStype_PING:
  case WStype_PONG:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
    break;
  }
}

/**
    Initial Web Socket
*/
void initWebSocket()
{
  SOCKET.readPreferences();

#ifdef _APP_DEBUG_ON_
  SOCKET.showPreferences();
#endif

  if (SOCKET.server != "" && SOCKET.port != "" && SOCKET.uri != "")
  {
    _APP_DEBUG_("WEB", "Starting Web Socket...()");

    // server address, port and URL
    webSocketClient.begin(SOCKET.server, SOCKET.port.toInt(), SOCKET.uri);

    // event handler
    webSocketClient.onEvent(webSocketEvent);

    // use HTTP Basic Authorization this is optional remove if not needed
    //webSocketClient.setAuthorization("user", "Password");

    // try ever 5000 again if connection has failed
    webSocketClient.setReconnectInterval(5000);

    SOCKET.status = true;
  }
  else
  {
    SOCKET.status = false;
  }
}

void sendBySocket(String json)
{
  if (SOCKET.status)
  {
    _APP_DEBUG_(F("Send by WS"), json);
    webSocketClient.sendTXT(json);
  }
}

bool wsConnected(){
  return SOCKET.connected;
}