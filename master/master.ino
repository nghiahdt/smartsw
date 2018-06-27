#include "nrf.h"

void setup()
{
	nrf_init();
	nrf_set_tx_mode();
}

unsigned char tx_buf[NRF_PAYLOAD_WIDTH] = { 'a', 'b', 'z'};

void loop()
{
	nrf_tx_packet(tx_buf);
	delay(1000);
}
