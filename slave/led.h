#ifndef _LED_H_
#define _LED_H_

#include "Arduino.h"

typedef enum LedType
{
	Led1 = PA3,
	Led2 = PB4,
	Led3 = PD1,
	LedStatus = PB5
} Led;

#endif