#include "nrf.h"

void setup()
{
	Serial.begin(115200);
	nrf_begin();
}

void loop()
{
	if(nrf24_dataReady())
	{
		uint8_t data[NRF_PAYLOAD_LENGTH];
		nrf24_getData(data);
		Serial.println();
		Serial.print(">Recieve: ");
		Serial.print((char*)data);
	}

	// send
	uint8_t data[NRF_PAYLOAD_LENGTH] = { 'a', 'b', 'c', NULL};
	nrf24_send(data);
	while(nrf24_isSending());
	if(nrf24_lastMessageStatus() == NRF24_TRANSMISSON_OK)
	{
		Serial.println();
		Serial.print("> Tranmission went OK");
	}
	nrf24_powerUpRx();

	Serial.print(".");
	delay(1000);
}
