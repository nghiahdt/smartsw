#define SERIAL_DEBUG true

#include <time.h>
#include <alexa.h>
#include <autowifi.h>
#include <save.h>

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
	auto wifiname = "wifiname";
	auto wifipass = "wifipass";
	AutoWifi::getInstance()->setValue(toStdString(WIFIAP_NAME), WIFIAP_PASS, WIFIAP_CHANNEL, WIFI_DEFAULT_NAME, WIFI_DEFAULT_PASS, [=]() {
		return std::string("wifi"); //cheat
		return std::string(Save::getInstance()->getString(wifiname));
	}, [=]() {
		return std::string("Ab@1234@"); //cheat
		return std::string(Save::getInstance()->getString(wifipass));
	});
	AutoWifi::getInstance()->setPairValue([=](const std::string& name, const std::string& pass, const std::string& account) {
		Save::getInstance()->setString(wifiname, name.c_str());
		Save::getInstance()->setString(wifipass, pass.c_str());
		Save::getInstance()->setString("account", account.c_str());
	}, toStdString(ID::get()));
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