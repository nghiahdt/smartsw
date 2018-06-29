#include "relay.h"
#include "button.h"
#include "nrf.h"

void setup()
{
	//relay_init(Relay1);
	//button_init(Button1);
	nrf_begin();
}

void loop()
{
	//if (button_just_pressed(Button1))
	//{
	//	relay_change(Relay1);
	//}

	//uint8_t data[NRF_PAYLOAD_LENGTH] = { 'a', 'b', 'c', 0};
	//nrf24_send(data);
	//while(nrf24_isSending());

	delay(1000);
}
