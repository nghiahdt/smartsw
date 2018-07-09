#define SERIAL_DEBUG true

#include <time.h>
#include <alexa.h>
#include <autowifi.h>

#include "conf.h"
#include "func.h"
#include "device.h"

void setup()
{
	Serial.begin(115200);
	initWifi();
	initAlexa();
}

void loop()
{
	int dt = getDt();
	AutoWifi::getInstance()->loop();
	AlexaSwitchManager::getInstance()->loop();
	HubManager::getInstance()->loop(dt);
}

void initWifi()
{
	AutoWifi::getInstance()->setValue(toStdString(WIFIAP_NAME), WIFIAP_PASS, WIFIAP_CHANNEL, WIFI_DEFAULT_NAME, WIFI_DEFAULT_PASS, []() {
		return std::string("wifi");
	}, []() {
		return std::string("Ab@1234@");
	});
	AutoWifi::getInstance()->setWaitFunc([](int dt) {
		delay(dt);
	});
}

void initAlexa()
{
	AutoWifi::getInstance()->addCallbackConnected([](){
		Serial.println("Add Alexa devices");
		AlexaSwitchManager::getInstance()->begin();
		AlexaSwitchManager::getInstance()->clearDevice();
		{
			static AlexaSwitch newHub(999);
			AlexaSwitchManager::getInstance()->addDevice(newHub.begin("new", [](){ 
				HubManager::getInstance()->setAdNewhub(true);
				Serial.println("Start add new hub");
			}, [](){ 
				HubManager::getInstance()->setAdNewhub(false);
				Serial.println("Stop add new hub");
			}));
		}
		{
			static AlexaSwitch all(1000);
			AlexaSwitchManager::getInstance()->addDevice(all.begin("all", [](){ 
				HubManager::getInstance()->setAllRelayWant(Relay::Status::On);
				Serial.println("Turn on all");
				//HubManager::getInstance()->setRelayWant("45910", 1, Relay::Status::On);
			}, [](){ 
				HubManager::getInstance()->setAllRelayWant(Relay::Status::Off);
				Serial.println("Turn off all");
				//HubManager::getInstance()->setRelayWant("45910", 1, Relay::Status::Off);
			}));
		}
	});
}