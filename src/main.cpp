
// platformio run --target uploadfs

#include <Arduino.h>
#include <global.h>

#if defined(_APP_DEBUG_ON_)
#include <debug.cpp>
#endif

#include <tools.cpp>


//bool waitMessageInComing = false;

#include <buttons.cpp>

#include <model/preferences.cpp>
#include <storage/flash.cpp>
#include <net/wifi.cpp>
#include <net/socket.cpp>
#include <net/mqtt.cpp>
#include <net/server.cpp>



#define Threshold 40 /* Greater the value, more the sensitivity */


void setup()
{

	// SERIAL
	Serial.begin(115200);

	// FILES
	mountFlash();
	openFiles();
	showMemory();
	getFreeSpiMemory();
	// SCAN NETWORKS
	//scanNetworks();

	// INIT AP
	//cleanApPreferences();
	initWifi();

	// INIT SCOKET
	initWebSocket();


	// INIT MQTT
	//initMqtt();

	// INIT WEB SERVER
	initWebServer();

	//initNow();
	//initBroadcastSlave();

	wait(100);

	touchAttachInterrupt(T0, resetButton_Flag, Threshold);
}

void loop()
{

	if (SOCKET.status){ 		
		webSocketClient.loop();
	}


	if (MQTT.status){
		mqtt_client.loop();
	}


	//wait(1000);

	//_APP_DEBUG_VALUE_("MEMORY", "Free: ", ESP.getFreeHeap());

	if(btnFactoryDefault){
		_APP_DEBUG_("TOUCHPAD VALUE", touchRead(4));
		resetButton();	
	}

	// for(int i = 0 ; i < stationsCount ; i++){
	// 	printMacStation(stations[i].peer_addr);
	// }

}