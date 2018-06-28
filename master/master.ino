#include "nrf.h"

void setup()
{
	nrf_init();
	nrf_set_rx_mode();
	Serial.begin(115200);
}

unsigned char rx_buf[NRF_PAYLOAD_WIDTH] = { '1', '2', '3', NULL };

void loop()
{
	if (nrf_rx_packet(rx_buf))
	{
		Serial.println((char*)rx_buf);
	}
	else
	{
		Serial.println(nrf_get_status());
	}
	delay(1000);
}
