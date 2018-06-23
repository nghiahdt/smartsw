#include "relay.h"
#include "button.h"

void setup()
{
	relay_init(Relay1);
	button_init(Button1);
}

void loop()
{
	if (button_just_pressed(Button1))
	{
		relay_change(Relay1);
	}
}
