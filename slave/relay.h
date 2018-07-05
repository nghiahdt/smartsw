#ifndef _RELAY_H_
#define _RELAY_H_

#include "Arduino.h"

typedef enum RelayType
{
	Relay1 = 0,
	Relay2,
	Relay3,
	RelayCount,
} Relay;

#define RELAY_PIN	{ PC7, PD2, PD3 }
#define RELAY_ON	LOW

void relay_init(Relay relay);
void relay_on(Relay relay);
void relay_off(Relay relay);
bool relay_is_on(Relay relay);
void relay_change(Relay relay);

#endif