#ifndef _NRF_H_
#define _NRF_H_

#define NRF_CE_PIN			D0 // Chip Enable
#define NRF_CSN_PIN			D1 // SPI Chip select
#define NRF_SCK_PIN			D2	
#define NRF_MOSI_PIN		D3
#define NRF_MISO_PIN		D4

#define nrf_delay_us(value)	delayMicroseconds(value)
#define NRF_SPI_DELAY_US	5 // 100 kbps

#define NRF_PAYLOAD_WIDTH	32	// from 0 to 32
#define NRF_ADDRESS_WIDTH	3	// from 3 to 5
#define NRF_TX_ADDRESS		{ 0x34, 0x43, 0x10 }
#define NRF_RX_ADDRESS		{ 0x34, 0x43, 0x10 }

void nrf_init();
void nrf_set_rx_mode();
void nrf_set_tx_mode();
unsigned char nrf_get_status();
unsigned char nrf_rx_packet(unsigned char* rx_buf);
void nrf_tx_packet(unsigned char* tx_buf);

#endif
