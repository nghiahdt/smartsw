#include "func.h"
#include <ESP8266WiFi.h>

const String& ID::get()
{
	static String sID = "";
	if (sID.length() <= 0)
	{
		uint32_t chipId = ESP.getChipId();
		uint16_t id = (chipId >> 16) ^ (uint16_t)chipId;
		sID = String(id);
	}
	return sID;
}

int getDt()
{
	static uint32_t lastTime = 0;
	uint32_t nowTime = millis();
	int dt = nowTime - lastTime;
	lastTime = nowTime;
	return dt;
}

std::string toStdString(const String& input)
{
  int inputLength = input.length() + 1;
  char c_input[inputLength];
  input.toCharArray(c_input, inputLength);
  return c_input;
}
