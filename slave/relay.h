#ifndef _RELAY_H_
#define _RELAY_H_

#include "Arduino.h"

typedef enum RelayType
{
	Relay1 = PB5, //LED_BUILTIN
	Relay2 = PB4,
	Relay3 = PC3,
	Relay4 = PC4,
	Relay5 = PC5
} Relay;

#define RELAY_ON LOW

void relay_init(Relay relay);
void relay_on(Relay relay);
void relay_off(Relay relay);
bool relay_is_on(Relay relay);
void relay_change(Relay relay);

#endif