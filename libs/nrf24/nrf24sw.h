#define NRF_PAYLOAD_LENGTH	32
#define NRF_CHANNEL			64
#define NRF24_CONFIG_CRC_2_BYTE
#define NRF24_ADDR_LEN 5
#define NRF24_SEND_TIME		100
uint8_t tx_address[NRF24_ADDR_LEN] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[NRF24_ADDR_LEN] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t nrf_data_send[NRF_PAYLOAD_LENGTH];
uint8_t nrf_data_get[NRF_PAYLOAD_LENGTH];

#include "nrf24.h"

void nrf_begin()
{
	nrf24_init();
	nrf24_config(NRF_CHANNEL, NRF_PAYLOAD_LENGTH);
	nrf24_tx_address(tx_address);
	nrf24_rx_address(rx_address);
}

#ifdef STM8S_MCU
bool nrf24_sendString(const char* text)
{
	bool endSend = true;
	bool ok = false;
	unsigned long begin = millis();
	for (int i=0; i<NRF_PAYLOAD_LENGTH; i++)
	{
		char c = text[i];
		if (c)
		{
			nrf_data_send[i] = c;
		}
		else
		{
			nrf_data_send[i] = 0;
			break;
		}
	}
#else
bool nrf24_sendString(String text, bool endSend = true)
{
	bool ok = false;
	unsigned long begin = millis();
	text.toCharArray((char*)nrf_data_send, NRF_PAYLOAD_LENGTH);
#endif
	while (!ok && (unsigned long)(millis() - begin) < NRF24_SEND_TIME)
	{
		nrf24_send(nrf_data_send);
		while(nrf24_isSending());
		ok = nrf24_lastMessageStatus() == NRF24_TRANSMISSON_OK;
	};
	if (endSend)
	{
		nrf24_powerUpRx();
	}
	return ok;
}

const char* nrf24_getString()
{
	for (int i=0; i<NRF_PAYLOAD_LENGTH; i++)
	{
		nrf_data_get[i] = 0;
	}
	if (nrf24_dataReady())
	{
		nrf24_getData(nrf_data_get);
	}
	return (const char*)nrf_data_get;
}

#ifndef STM8S_MCU
#define OUTPUT_FAST OUTPUT
#endif

void nrf24_setupPins()
{
	pinMode(NRF_CE_PIN, OUTPUT_FAST);
	pinMode(NRF_CSN_PIN, OUTPUT_FAST);
	pinMode(NRF_SCK_PIN, OUTPUT_FAST);
	pinMode(NRF_MOSI_PIN, OUTPUT_FAST);
	pinMode(NRF_MISO_PIN, INPUT_PULLUP);
}

void nrf24_ce_digitalWrite(uint8_t state)
{
	digitalWrite(NRF_CE_PIN, state);
	delayMicroseconds(1);
}

void nrf24_csn_digitalWrite(uint8_t state)
{
	digitalWrite(NRF_CSN_PIN, state);
	delayMicroseconds(1);
}

void nrf24_sck_digitalWrite(uint8_t state)
{
	digitalWrite(NRF_SCK_PIN, state);
	delayMicroseconds(1);
}

void nrf24_mosi_digitalWrite(uint8_t state)
{
	digitalWrite(NRF_MOSI_PIN, state);
	delayMicroseconds(1);
}

uint8_t nrf24_miso_digitalRead()
{
	delayMicroseconds(1);
	return digitalRead(NRF_MISO_PIN);
}