#define STM8S_MCU

#define NRF_CE_PIN			PD3
#define NRF_CSN_PIN			PC4
#define NRF_SCK_PIN			PD2	
#define NRF_MOSI_PIN		PD4
#define NRF_MISO_PIN		PC6

#define NRF_PAYLOAD_LENGTH	32
#define NRF_CHANNEL			64
#define NRF24_CONFIG_CRC_2_BYTE
#define NRF24_ADDR_LEN		5
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