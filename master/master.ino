#define SERIAL_DEBUG true

#include "nrf.h"

void setup()
{
	Serial.begin(115200);
	Nrf.begin([](const std::string& text) {
		if (SERIAL_DEBUG)
		{
			Serial.println(String("Recieve: ") + text.c_str());
		}
	});
}

int count = 0;
void loop()
{
	if (Nrf.send("{\"x\":45910,\"m\":4444,\"c\":" + String(count%2) + "}"))
	{
		if (SERIAL_DEBUG)
		{
			Serial.println("Tranmission went OK");
		}
		count++;
	}
	delay(1000);
}
