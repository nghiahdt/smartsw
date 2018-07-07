#define SERIAL_DEBUG true

#include <alexa.h>

#include "nrf.h"
#include "wifi.h"
#include "device.h"


void setup()
{
	Serial.begin(115200);
	Nrf.begin([](const std::string& text) {
		HubManager::getInstance()->updateStatus(text, true);
	});
	initAlexa();
}

int count = 0;
void loop()
{
	int dt = getDt();
	Wifi::getInstance()->loop();
}

void wait(int dt)
{
	delay(dt);
}

int getDt()
{
	static uint32_t lastTime = 0;
	uint32_t nowTime = millis();
	int dt = nowTime - lastTime;
	lastTime = nowTime;
	return dt;
}

void initAlexa()
{
	static AlexaSwitch light(77);
	Wifi::getInstance()->addCallbackConnected([](){
		AlexaSwitchManager::getInstance()->begin();
		light.begin("light", [](){ 
			HubManager::getInstance()->setRelayWant("45910", 1, Relay::Status::On);
		}, [](){ 
			HubManager::getInstance()->setRelayWant("45910", 1, Relay::Status::Off);
		});
		AlexaSwitchManager::getInstance()->clearDevice();
		AlexaSwitchManager::getInstance()->addDevice(&light);
	});
	Wifi::getInstance()->addCallbackLoop([](){
		AlexaSwitchManager::getInstance()->loop();
	});
}
