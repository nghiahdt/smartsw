#include "relay.h"
#include "button.h"
#include "nrf.h"

void setup()
{
	relay_init(Relay1);
	//button_init(Button1);
	nrf_begin();
}

void loop()
{
	nrf_loop();

	//if (button_just_pressed(Button1))
	{
	relay_change(Relay1);
	}

	delay(1000);
}