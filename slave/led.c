#include "led.h"

int LedPin[LedCount] = LED_PIN;
int BlinkTime[LedCount];

void led_init(Led led)
{
	pinMode(LedPin[led], OUTPUT);
	led_off(led);
	BlinkTime[led] = 0;
}

void led_on(Led led)
{
	digitalWrite(LedPin[led], LED_ON);
}

void led_off(Led led)
{
	digitalWrite(LedPin[led], !LED_ON);
}

bool led_is_on(Led led)
{
	return digitalRead(LedPin[led]) == LED_ON;
}

void led_change(Led led)
{
	if (led_is_on(led))
	{
		led_off(led);
	}
	else
	{
		led_on(led);
	}
}

void led_blink(Led led, int dt)
{
	BlinkTime[led] += dt;
	if (BlinkTime[led] >= LED_BLINK_TICK)
	{
		BlinkTime[led] = 0;
		led_change(led);
	}
}