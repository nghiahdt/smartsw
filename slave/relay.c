#include "relay.h"

int RelayPin[RelayCount] = RELAY_PIN;

void relay_init(Relay relay)
{
	pinMode(RelayPin[relay], OUTPUT);
	relay_off(relay);
}

void relay_on(Relay relay)
{
	digitalWrite(RelayPin[relay], RELAY_ON);
}

void relay_off(Relay relay)
{
	digitalWrite(RelayPin[relay], !RELAY_ON);
}

bool relay_is_on(Relay relay)
{
	return digitalRead(RelayPin[relay]) == RELAY_ON;
}

void relay_change(Relay relay)
{
	if (relay_is_on(relay))
	{
		relay_off(relay);
	}
	else
	{
		relay_on(relay);
	}
}