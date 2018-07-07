#define SERIAL_DEBUG true

#include <time.h>
#include <alexa.h>
#include <autowifi.h>

#include "conf.h"
#include "func.h"
#include "nrf.h"
#include "device.h"

void setup()
{
	Serial.begin(115200);
	Nrf.begin([](const std::string& text) {
		HubManager::getInstance()->updateStatus(text, true);
	});
	initWifi();
	initAlexa();
}

void loop()
{
	int dt = getDt();
	//
}

int getDt()
{
	static uint32_t lastTime = 0;
	uint32_t nowTime = millis();
	int dt = nowTime - lastTime;
	lastTime = nowTime;
	return dt;
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
	static AlexaSwitch light(77);
	AutoWifi::getInstance()->addCallbackConnected([](){
		AlexaSwitchManager::getInstance()->begin();
		light.begin("light", [](){ 
			HubManager::getInstance()->setRelayWant("45910", 1, Relay::Status::On);
		}, [](){ 
			HubManager::getInstance()->setRelayWant("45910", 1, Relay::Status::Off);
		});
		AlexaSwitchManager::getInstance()->clearDevice();
		AlexaSwitchManager::getInstance()->addDevice(&light);
	});
	AutoWifi::getInstance()->addCallbackLoop([](){
		AlexaSwitchManager::getInstance()->loop();
	});
}
