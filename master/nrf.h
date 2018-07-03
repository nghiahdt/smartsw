#define NRF_CE_PIN			D0
#define NRF_CSN_PIN			D1
#define NRF_SCK_PIN			D2	
#define NRF_MOSI_PIN		D3
#define NRF_MISO_PIN		D7

#define NRF_PAYLOAD_LENGTH	32
#define NRF_CHANNEL			64
#define NRF24_CONFIG_CRC_2_BYTE
#define NRF24_ADDR_LEN 5
uint8_t tx_address[NRF24_ADDR_LEN] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[NRF24_ADDR_LEN] = {0xD7,0xD7,0xD7,0xD7,0xD7};

#include <nrf24.h>

void nrf_begin()
{
  nrf24_init();
  nrf24_config(NRF_CHANNEL, NRF_PAYLOAD_LENGTH);
  nrf24_tx_address(tx_address);
  nrf24_rx_address(rx_address);
}

void nrf24_setupPins()
{
	pinMode(NRF_CE_PIN, OUTPUT);
	pinMode(NRF_CSN_PIN, OUTPUT);
	pinMode(NRF_SCK_PIN, OUTPUT);
	pinMode(NRF_MOSI_PIN, OUTPUT);
	pinMode(NRF_MISO_PIN, INPUT_PULLUP);
}
         
void nrf24_ce_digitalWrite(uint8_t state)
{
	digitalWrite(NRF_CE_PIN, state);
}

void nrf24_csn_digitalWrite(uint8_t state)
{
	digitalWrite(NRF_CSN_PIN, state);
}

void nrf24_sck_digitalWrite(uint8_t state)
{
	digitalWrite(NRF_SCK_PIN, state);
}

void nrf24_mosi_digitalWrite(uint8_t state)
{
	digitalWrite(NRF_MOSI_PIN, state);
}

uint8_t nrf24_miso_digitalRead()
{
    return digitalRead(NRF_MISO_PIN);
}
