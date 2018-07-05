#ifndef _LED_H_
#define _LED_H_

#include "Arduino.h"

typedef enum LedType
{
	Led1 = 0,
	Led2,
	Led3,
	LedStatus,
	LedCount,
} Led;

#define LED_PIN			{ PA3, PB4, PD1, PB5 }
#define LED_ON			LOW
#define LED_BLINK_TICK	100

void led_init(Led led);
void led_on(Led led);
void led_off(Led led);
bool led_is_on(Led led);
void led_change(Led led);
void led_blink(Led led, int dt);

#endif
