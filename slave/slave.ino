#include "relay.h"
#include "button.h"
#include "nrf.h"

void setup()
{
	//relay_init(Relay1);
	//button_init(Button1);
	nrf_init();
	nrf_set_tx_mode();
}

unsigned char tx_buf[NRF_PAYLOAD_WIDTH] = { 'a', 'b', 'z'};

void loop()
{
	//if (button_just_pressed(Button1))
	//{
	//	relay_change(Relay1);
	//}
	nrf_tx_packet(tx_buf);
	delay(1000);
}
