#include <nrf24.h>
#include "nrf24def.h"

uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};

void setup()
{
	Serial.begin(115200);
	nrf24_init();
	nrf24_config(NRF_CHANNEL, NRF_PAYLOAD_LENGTH);
	nrf24_tx_address(tx_address);
	nrf24_rx_address(rx_address);
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
