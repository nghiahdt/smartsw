#define SERIAL_DEBUG true

#include "nrf.h"
#include "wifi.h"
#include <alexa.h>

void setup()
{
	Serial.begin(115200);
	Nrf.begin([](const std::string& text) {
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
	static AlexaSwitch light(99);
	Wifi::getInstance()->addCallbackConnected([](){
		AlexaSwitchManager::getInstance()->begin();
		light.begin("light", [](){ 
			Nrf.send("{\"x\":45910,\"m\":4444,\"c\":212}");
		}, [](){ 
			Nrf.send("{\"x\":45910,\"m\":4444,\"c\":202}");
		});
		AlexaSwitchManager::getInstance()->clearDevice();
		AlexaSwitchManager::getInstance()->addDevice(&light);
	});
	Wifi::getInstance()->addCallbackLoop([](){
		AlexaSwitchManager::getInstance()->loop();
	});
}