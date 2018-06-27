#include "nrf.h"

void setup()
{
	nrf_init();
	nrf_set_tx_mode();
	Serial.begin(115200);
}

unsigned char tx_buf[NRF_PAYLOAD_WIDTH] = { 'a', 'b', 'z'};

void loop()
{
	Serial.println(nrf_get_status());
	nrf_tx_packet(tx_buf);
	delay(1000);
}
