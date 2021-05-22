 // "\" 

#include <WiFi.h>
#include "esp_wifi.h"
#include <esp_now.h>
// #include <ArduinoJson.h>

#include <model/net/wifi.cpp>
#include <model/net/now.cpp>

#define NUM_STATION 20 // ESP-Now puede manejar un máximo de 20 estaciones.

int stationsCount = 0;                     // Lleva la cuenta del numero de estaciones pareadas
esp_now_peer_info_t stations[NUM_STATION]; // Almacena la información de cada uno de las estaciones que se agrega como par

//esp_now_peer_info_t station;

//esp_err_t unregistred ;

void printMacStation(const uint8_t *mac_addr_station);

void initApSta();
void initAp();
void initWifi();

void initNow();
// void initBroadcastSlave();

void stationParing(const uint8_t *mac_addr_station);
void sendMessageRecvOk(String macStr);
bool manageStation();
void sendMsgToStation(String json);

bool wsConnected(); //websokcet
bool mqttStatus();

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int incomingData_len);

void sendBySocket(String json);

WifiPoint AP;
Now NOW;

String message = "no message in memory";
bool waitResq = false;
//uint8_t broadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//uint8_t broadcastAddress[] = { 0x30, 0xAE, 0xA4, 0xDD, 0x94, 0xC0 }; //Original
//uint8_t broadcastAddress[] = { 0x24, 0x62, 0xAB, 0xF3, 0xAD, 0x54 }; //AZ

void printMacStation(const uint8_t *mac_addr_station)
{

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            (int)mac_addr_station[0], (int)mac_addr_station[1], (int)mac_addr_station[2], (int)mac_addr_station[3], (int)mac_addr_station[4], (int)mac_addr_station[5]);

    _APP_DEBUG_("MAC STATION :: ", macStr);
}


// Registra las estaciones conectadas al AP, 
// y lo agrega a la lista de estaciones
void registreStations()
{
    _APP_DEBUG_("registreStations()","CLIENTES CONECTADOS");

    wifi_sta_list_t stationList;

    esp_wifi_ap_get_sta_list(&stationList);

    _APP_DEBUG_("N of connected stations: ", stationList.num);

    for (int i = 0; i < stationList.num; i++)
    {
        wifi_sta_info_t station = stationList.sta[i];

        printMacStation(station.mac);

        stationParing(station.mac);

        //Enviar mensaje por ESPNOW, validar respuesta
        //Si no se recibe desvincular de la red EPNOW
        _APP_DEBUG_("TO DO Verificar paring enviando un mensaje de prueba","");
        // char macStr[18];
        // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
        //      station.mac[0], station.mac[1], station.mac[2], station.mac[3], station.mac[4], station.mac[5]);
        // sendMessageRecvOk(macStr);


    }
}

/*
*   EVENTOS WIFI
*/
void WiFiEvent(WiFiEvent_t event)
{
    _APP_DEBUG_(F("[WiFi-event] event: "), event);

    switch (event)
    {
    case SYSTEM_EVENT_WIFI_READY:
        _APP_DEBUG_(F("WiFi interface ready"),"");
        break;
    case SYSTEM_EVENT_SCAN_DONE:
        	_APP_DEBUG_(F("Completed scan for access points"),"");
        break;
    case SYSTEM_EVENT_STA_START:
        Serial.println("WiFi client started");
        break;
    case SYSTEM_EVENT_STA_STOP:
        Serial.println("WiFi clients stopped");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        _APP_DEBUG_(F("Connected to access point"),"");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("Disconnected from WiFi access point");
        break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        Serial.println("Authentication mode of access point has changed");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        _APP_DEBUG_(F("Obtained IP address:"), WiFi.localIP());
        break;
    case SYSTEM_EVENT_STA_LOST_IP:
        Serial.println("Lost IP address and IP address is reset to 0");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
        Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
        Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
        break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
        Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
        break;
    case SYSTEM_EVENT_AP_START:
        _APP_DEBUG_(F("WiFi access point started"),"");
        break;
    case SYSTEM_EVENT_AP_STOP:
         _APP_DEBUG_(F("WiFi access point  stopped"),"");
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        /***************************/
        _APP_DEBUG_(F("Client connected"), F("Try registre station"));
        // Si el cliente / estacion tiene los credenciales para conectarse a la red, se almacena como par admitido en el nodo
        registreStations();
        
        /***************************/
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
          _APP_DEBUG_(F("Client disconnected"), "");
        break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
        _APP_DEBUG_(F("Assigned IP address to client"),"");
        break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
        Serial.println("Received probe request");
        break;
    case SYSTEM_EVENT_GOT_IP6:
        Serial.println("IPv6 is preferred");
        break;
    case SYSTEM_EVENT_ETH_START:
        Serial.println("Ethernet started");
        break;
    case SYSTEM_EVENT_ETH_STOP:
        Serial.println("Ethernet stopped");
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        Serial.println("Ethernet connected");
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        Serial.println("Ethernet disconnected");
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        _APP_DEBUG_(F("Obtained IP address"),"");
        break;
    default:
        break;
    }
}

/**
 *  AP 
 */
void initWifi()
{
    _APP_DEBUG_("WIFI", "initWifi()");
    // WiFi.mode(WIFI_AP_STA); // [WIFI_STA | WIFI_AP | WIFI_AP_STA]

    // Examples of different ways to register wifi events
    WiFi.onEvent(WiFiEvent);
    // WiFi.onEvent(WiFiGotIP, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    // WiFiEventId_t eventID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
    //     Serial.print("WiFi lost connection. Reason: ");
    //     Serial.println(info.disconnected.reason);
    // }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    AP.readPreferences();

#ifdef _APP_DEBUG_ON_
    AP.showPreferences();
#endif

    if (AP.ssid == "" && AP.pass == "")
    {
        //Primer arranque configuracion del dispositivo
        _APP_DEBUG_("WIFI", "Primer arranque configuracion del dispositivo");
        _APP_DEBUG_("AP", "Using Default Values");
        //AP.showPreferences();
        initAp();
    }
    else
    {
        _APP_DEBUG_("WIFI", "Aplicando configuracion EEPROM");
        //NOW.showPreferences(); //aqui ¿?
        initApSta();
    }

} //networkInit

// Primer Arranque
void initAp()
{
    _APP_DEBUG_("WIFI", "Soft AP");
    WiFi.mode(WIFI_AP); // [WIFI_STA | WIFI_AP | WIFI_AP_STA]

    AP.ssid = ("[ESP_AP]" + WiFi.macAddress()).c_str();
    WiFi.softAP(AP.ssid.c_str(), WIFI_PASS_AP_DEFAULT, WIFI_CHAN_AP_DEFAULT, false, 4);
    _APP_DEBUG_VALUE_("WIFI", "AP IP address: ", WiFi.softAPIP());
}

void initApSta()
{
    _APP_DEBUG_("WIFI", "Soft [AP] STA");

    // ToDo :: Investigar el el envio por el modo AP
    // se configura tambien en esp now   stations.ifidx = ESP_IF_WIFI_STA;
    // probar en ESP_IF_WIFI_AP.

    WiFi.mode(WIFI_AP_STA); // [WIFI_STA | WIFI_AP | WIFI_AP_STA]

    //WiFi.mode(WIFI_STA);

    // Conection
    unsigned int intent = 1;
    WiFi.begin(AP.ssid.c_str(), AP.pass.c_str());
    while (WiFi.status() != WL_CONNECTED && intent <= 15)
    {
        wait(1000);
        _APP_DEBUG_VALUE_("WIFI", "Intent Conection: ", intent++);
    }

    if (intent > 15)
    {
        _APP_DEBUG_(F("WIFI"), F("FAIL Configuration in the Wi-Fi AP."));
        _APP_DEBUG_(F("Disconnected from WiFi access point"), F("EXEC --> WiFi.disconnect()"));
        WiFi.disconnect(); //en caso de no conectarse a la wifi, se desconecta para que se pueda volver a configurar por la web
    }
    else
    {
        _APP_DEBUG_VALUE_("WIFI", "Station IP Address: ", WiFi.localIP());
        _APP_DEBUG_VALUE_("WIFI", "Station MAC Address: ", WiFi.macAddress());
        _APP_DEBUG_VALUE_("WIFI", "Station Channel: ", WiFi.channel());
    }

    //Y si llamanos a todos los read para que esten disponibles las configuraciones desde cualquiera ambito
    NOW.readPreferences();
#ifdef _APP_DEBUG_ON_
    Serial.println(F(""));
    NOW.showPreferences();
#endif

    if (NOW.ssid != "" && NOW.pass != "" && AP.chan != "")
    {
        // Define al dispositivo como punto de acceso, true ocultar SSID
        WiFi.softAP(NOW.ssid.c_str(),
                    NOW.pass.c_str(),
                    AP.chan.toInt(), false, 4);

        _APP_DEBUG_VALUE_("WIFI", "AP  IP  Address:", WiFi.softAPIP());
        _APP_DEBUG_VALUE_("WIFI", "STA MAC Address:", WiFi.macAddress());
        _APP_DEBUG_VALUE_("WIFI", "AP  MAC Address:", WiFi.softAPmacAddress());

        initNow();
        //initBroadcastSlave();
    }
    else
    {
        _APP_DEBUG_("WIFI", "[W] No hay configuracion ESPNOW !!!");
    }

    Serial.println(F(""));
}


/**
 *  WiFi stations Scaner in AP Mode
 *  return: JSON Stringify
 */
/*
String scanForstations()
{
    _APP_DEBUG_("WIFI", "Scan for stations");

    int8_t scanResults = WiFi.scanNetworks();
    String json;

    if (scanResults == 0)
    {
        _APP_DEBUG_("WIFI", "No WiFi devices in AP Mode found");
    }
    else
    {
        _APP_DEBUG_VALUE_("WIFI", "Devices Found: ", scanResults);

        DynamicJsonDocument doc(2048);
        JsonArray apList = doc.to<JsonArray>();

        for (int i = 0; i < scanResults; i++)
        {
            WifiPoint slave;
            slave.ssid = WiFi.SSID(i);
            slave.mac = WiFi.RSSI(i);
            slave.chan = WiFi.channel(i);
            slave.showToJson(i);
            delay(10);

            JsonObject ap = apList.createNestedObject();
            ap["ssid"] = slave.ssid;
            ap["rssi"] = slave.mac;
            ap["ch"] = slave.chan;

            // clean up ram
            WiFi.scanDelete();

            serializeJson(doc, json);
            Serial.println(json);
        }
        Serial.println("");
    }

    return json;
}
*/

/**
 *  WiFi RssId Scaner
 *  return: JSON Stringify
 */
String scanNetworks()
{
    #ifdef _APP_DEBUG_ON
        Serial.println("");
    #endif
    _APP_DEBUG_(F("WIFI"), F("Scan Networks"));

    int8_t scanResults = WiFi.scanNetworks();
    String json;

    if (scanResults == 0)
    {
        _APP_DEBUG_(F("WIFI"), F("No WiFi Networks found"));
    }
    else
    {
        #ifdef _APP_DEBUG_ON
            Serial.println("");
        #endif
        _APP_DEBUG_VALUE_(F("WIFI"), F("Devices Found: "), scanResults);

        DynamicJsonDocument doc(2048);
        JsonArray apList = doc.to<JsonArray>();

        for (int i = 0; i < scanResults; i++)
        {

            WifiPoint apDevice;
            apDevice.ssid = WiFi.SSID(i);
            apDevice.mac = WiFi.BSSIDstr(i);
            int rssi = WiFi.RSSI(i);
            apDevice.chan = WiFi.channel(i);

#if defined(_APP_DEBUG_ON_)
            Serial.print(i + 1);
            Serial.print(F(": "));
            apDevice.show();
            Serial.print(F(": "));
            Serial.println(rssi);
            wait(5);
#endif

            JsonObject ap = apList.createNestedObject();
            ap["ssid"] = apDevice.ssid;
            ap["rssi"] = rssi;
            ap["ch"] = apDevice.chan;
            //wait(1);

        } //for

         #ifdef _APP_DEBUG_ON
            Serial.println("");
        #endif

        // wait(5);

        // clean up ram
        WiFi.scanDelete();

        wait(5);

        serializeJson(doc, json);
    }

    return json;
}//scanNetworks


void sendMessageRecvOk(String macStr){
    StaticJsonDocument<50> doc;
    String jsonOK;
    doc["mac"] = macStr;
    doc["msgRecv"] = "OK";

    serializeJson(doc, jsonOK);
    sendMsgToStation(jsonOK);
}//sendMessageRecvOk



/**
 *  ESP Now Callback when the data has been sent
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    _APP_DEBUG_VALUE_("NOW", "Last Packet Send To: ", macStr);
    _APP_DEBUG_VALUE_("NOW", "Last Packet Send Status:", status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

} //OnDataSent

/**
 *  ESP Now Callback when the data has been recived
 */
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int incomingData_len)
{

    // Comprobar que el emisor esta registrado
    _APP_DEBUG_(F("NOW"), F("Data On Recived"));
    bool par__registerd = esp_now_is_peer_exist(mac_addr);
    if (!par__registerd)
    {
        _APP_DEBUG_(F("Estacion NO registrada"), F("Stacion Rechazada"));
        return;
    }

        // if(waitMessageInComing){
        //         char buffer[incomingData_len]; //Json
        //         memcpy(&buffer, incomingData, incomingData_len);
        //         message = buffer;
        //         _APP_DEBUG_VALUE_("NOW WAIT MESSAGE FROM STATION ", "Data On Recived DATA", buffer);
        //         waitMessageInComing = false;

        //     return;
        // }



/*
    // for (int i = 0; i < stationsCount; i++)
    // {
    //     // printMacStation(stations[i].peer_addr);
    //     // printMacStation(mac_addr);

    //     char macStr1[18];
    //     snprintf(macStr1, sizeof(macStr1), "%02x:%02x:%02x:%02x:%02x:%02x",
    //              (int)stations[i].peer_addr[0], (int)stations[i].peer_addr[1], (int)stations[i].peer_addr[2], (int)stations[i].peer_addr[3], (int)stations[i].peer_addr[4], (int)stations[i].peer_addr[5]);

    //     //Serial.println(macStr1);

    //     char macStr2[18];
    //     snprintf(macStr2, sizeof(macStr2), "%02x:%02x:%02x:%02x:%02x:%02x",
    //              (int)mac_addr[0], (int)mac_addr[1], (int)mac_addr[2], (int)mac_addr[3], (int)mac_addr[4], (int)mac_addr[5]);

    //     //Serial.println(macStr2);

    //     if (strcmp(macStr1, macStr2) == 0)
    //     {
    //         par__registerd = true;
    //         break;
    //     }

    // }
*/

    _APP_DEBUG_(F("Estacion registrada"), F("OK -- Procesando mensaje"));

    //sendMsgToStation(json);

    char buffer[incomingData_len]; //Json
    memcpy(&buffer, incomingData, incomingData_len);

    _APP_DEBUG_VALUE_(F("NOW"), F("Data On Recived >>> "), buffer);

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    _APP_DEBUG_VALUE_(F("NOW"), F("Last Packet Recv from: "), macStr);
    _APP_DEBUG_VALUE_(F("NOW"), F("Last Packet Recv Data: "), incomingData_len);
    _APP_DEBUG_(F("NOW"), F("Management to send response..."));


    sendMessageRecvOk(macStr);

    if(String(buffer).indexOf("GPIO_STATUS") != -1){
        _APP_DEBUG_(F("Mensaje procesado"), F("GPIO_STATUS"));
        message = buffer;
        waitResq = false;
        return;
    }

    // Agraga la MAC del concentrador / MASTER para ser envida por WS e identificar el dato

    StaticJsonDocument<250> docIncoming;
    DeserializationError err = deserializeJson(docIncoming, buffer);
    String json;

    
    docIncoming["macServer"] = WiFi.macAddress();
    serializeJson(docIncoming, json);

    /*
    if (NOW.status)
        mqttPublish(json);
    */

    sendBySocket(json);
}//OnDataRecv

/**
 *  EspNow
 */
void initNow()
{
    //     NOW.readPreferences();

    // #ifdef _APP_DEBUG_ON_
    //     NOW.showPreferences();
    // #endif

    _APP_DEBUG_(F("ESPNOW"), F("initNow()"));

    #ifdef _APP_DEBUG_ON_
            Serial.println(F(""));
    #endif

    if (esp_now_init() == ESP_OK)
    {
        _APP_DEBUG_(F("NOW"), F("ESPNow Init Success"));

        esp_now_register_send_cb(OnDataSent);
        esp_now_register_recv_cb(OnDataRecv);

        NOW.status = true;
    }
    else
    {
        _APP_DEBUG_(F("NOW"), F("ESPNow Init Failed"));
        wait(1000);
        initNow(); //Reinicia configuracion
    }

}



/**
 *  ESP Now Delete Pair whit the master
 */
void deletePeer()
{
    //const esp_now_peer_info_t *peer = &stations;
    const uint8_t *peer_addr = stations[stationsCount].peer_addr;
    esp_err_t delStatus = esp_now_del_peer(peer_addr);
    String state;

    if (delStatus == ESP_OK)
    {
        state = "Success";
    }
    else if (delStatus == ESP_ERR_ESPNOW_NOT_INIT)
    {
        state = "Not Init";
    }
    else if (delStatus == ESP_ERR_ESPNOW_ARG)
    {
        state = "Invalid Argument";
    }
    else if (delStatus == ESP_ERR_ESPNOW_NOT_FOUND)
    {
        state = "Peer not foundt";
    }
    else
    {
        state = "Not sure what Happened";
    }

    _APP_DEBUG_VALUE_(F("NOW"), F("Slave Delete Status:"), state);
}

/**
 *  ESP Now Check if the stations is paired whit the master
 *  If not, pair the slave with master
 *  return: status
 */
bool manageStation()
{
        _APP_DEBUG_(F("NOW"), F("manageStation"));
        const esp_now_peer_info_t *peer = &stations[stationsCount];
        const uint8_t *peer_addr = stations[stationsCount].peer_addr;
        bool exists = esp_now_is_peer_exist(peer_addr);

        if (exists)
        {
            _APP_DEBUG_(F("NOW"), F("Already Paired"));
            return true;
        }
        else
        {
            esp_err_t addStatus = esp_now_add_peer(peer);

            if (addStatus == ESP_ERR_ESPNOW_EXIST)
            {
                _APP_DEBUG_(F("NOW"), F("Peer Exists"));
                return true;
            }
            else if (addStatus == ESP_OK)
            {
                _APP_DEBUG_(F("NOW"), F("Pair success"));
                stationsCount++;
                _APP_DEBUG_VALUE_("NOW", "NEW Station connected Count ", stationsCount);
                return true;
            }
            else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT)
            {
                _APP_DEBUG_("NOW", "Not Init");
                return false;
            }
            else if (addStatus == ESP_ERR_ESPNOW_ARG)
            {
                _APP_DEBUG_("NOW", "Invalid Argument");
                return false;
            }
            else if (addStatus == ESP_ERR_ESPNOW_FULL)
            {
                _APP_DEBUG_("NOW", "Peer list full");
                return false;
            }
            else if (addStatus == ESP_ERR_ESPNOW_NO_MEM)
            {
                _APP_DEBUG_("NOW", "Out of memory");
                return false;
            }
            else
            {
                _APP_DEBUG_("NOW", "Not sure what happened");
                return false;
            }
        }
}

/**
 *  ESP NOW registra las estaciones autorizadas
 */ 

void stationParing(const uint8_t *mac_addr_station)
{
    _APP_DEBUG_(F("ESPNOW"), F("stationParing()"));
    printMacStation(mac_addr_station);

    stations[stationsCount].channel = AP.chan.toInt();
    stations[stationsCount].encrypt = false;         // no encryption
    stations[stationsCount].ifidx = ESP_IF_WIFI_STA; // Wi-Fi interface that peer uses to send/receive ESPNOW data 

    memcpy(&stations[stationsCount].peer_addr, mac_addr_station, 6);

    manageStation();
}


void sendMsgToStation(String jsonString)
{
    String state = "";

    StaticJsonDocument<250> doc;
    DeserializationError err = deserializeJson(doc, jsonString);

    // int gpio = doc["gpio"];
    // Serial.print("GPIO :::");
    // Serial.println(gpio);
    // int value = doc["value"];
    // Serial.print("VALUE::::::: ");
    // Serial.println(value);
    String macToSend =  doc["mac"];
    _APP_DEBUG_ (F("GET MACSTATION FROM JSON MESSAGE >>> "), macToSend);

    // Convertir mac String --> Bytes
    //const char* macStr =  "24:62:AB:F3:08:D4";
    const char* macStr =  macToSend.c_str();

    uint8_t mac_addr_station[6];

    parseBytes(macStr, ':', mac_addr_station, 6, 16);

    _APP_DEBUG_(F("TO SEND a station"), jsonString);

    // const uint8_t *peer_addr = mac_addr_station;
    esp_err_t result = esp_now_send(mac_addr_station, (uint8_t *)jsonString.c_str(), jsonString.length() + 1);

    if (result == ESP_OK)
    {
        state = "Success";
    }
    else if (result == ESP_ERR_ESPNOW_NOT_INIT)
    {
        state = "ESP_ERR_ESPNOW_NOT_INIT";
    }
    else if (result == ESP_ERR_ESPNOW_ARG)
    {
        state = "ESP_ERR_ESPNOW_ARG";
    }
    else if (result == ESP_ERR_ESPNOW_INTERNAL)
    {
        state = "ESP_ERR_ESPNOW_INTERNAL";
    }
    else if (result == ESP_ERR_ESPNOW_NO_MEM)
    {
        state = "ESP_ERR_ESPNOW_NO_MEM";
    }
    else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
    {
        state = "Error Peer not found.";
    }
    else
    {
        state = "Not sure what Hapened";
    }

    _APP_DEBUG_VALUE_(F("NOW"), F("Send Status:"), state);

}



String topology(){

    _APP_DEBUG_(F("topology()"), "")
    
    DynamicJsonDocument doc(1024);
    JsonArray station = doc.to<JsonArray>();
    String json;

    char macStr[18];

    //Recorre las staciones recoge su mac y consulta los GPIO
    for(int i = 0 ; i < stationsCount ; i++){
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                 (int)stations[i].peer_addr[0], (int)stations[i].peer_addr[1], (int)stations[i].peer_addr[2], (int)stations[i].peer_addr[3], (int)stations[i].peer_addr[4], (int)stations[i].peer_addr[5]);

        Serial.println(macStr);
        //sendMessageRecvOk(macStr);

        StaticJsonDocument<250> doc;
        String json;
        doc["mac"] = macStr;
        doc["function"] = "GPIO_STATUS";
        serializeJson(doc, json);

        sendMsgToStation(json);

        do{
            wait(80);
        }while(waitResq);

        _APP_DEBUG_(F("waitResqt\t"), waitResq);
        JsonObject st = station.createNestedObject();
        st["mac"] = macStr;

        StaticJsonDocument<125> msg;
        deserializeJson(msg, message);

        st["deviceName"] = msg["DEVICE_NAME"];
        st["gpioStatus"] = msg["GPIO_STATUS"];

        message = "";

    }
    

    serializeJson(doc, json);

    _APP_DEBUG_(F("[ >>> ] topology()"), json);

    return json;


}


#ifdef __cplusplus
extern "C" {
#endif

uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif


String deviceStatus(){

  
    StaticJsonDocument<1024> doc;
    String json;
    doc["network"] = WiFi.status() == 3 ? true : false;
    doc["ip"] = WiFi.localIP().toString();
    doc["mac"] = WiFi.macAddress();
    doc["macAP"] = WiFi.softAPmacAddress();
    doc["txPower"] = String(WiFi.getTxPower()) + " dbm";
    doc["rssi"] = String(WiFi.RSSI()) + " dbm";

    doc["espnow"] = NOW.status;
    doc["stations"] = stationsCount;
    doc["websocket"] = wsConnected();
    doc["mqtt"] = mqttStatus();
   

    doc["chipRev"] = ESP.getChipRevision();
    doc["model"] = CHIP;
    doc["sdk"] = ESP.getSdkVersion();
    doc["fwRev"] = FW_REVISON;
    doc["freeRam"] = String(ESP.getFreeHeap() / 1024) + " Kb";
    doc["freeSpi"] = String(getFreeSpiMemory() /1024) + " Kb";
    doc["freqCpu"] = String(ESP.getCpuFreqMHz()) + " Mhz";


    char tmpCpu[5];
    snprintf(tmpCpu, sizeof(tmpCpu), "%02f", ((temprature_sens_read() - 32) / 1.8) ); //Covierte a celsius

    //doc["tmpCpu"] = String(atof(tmpCpu);    //°C
    doc["tmpCpu"] = String(tmpCpu) + " °C";

    serializeJson(doc, json);

    return json;

}