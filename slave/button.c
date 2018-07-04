#include "button.h"
#include "nrf.h"

int ButtonPin[ButtonCount] = BUTTON_PIN;
bool ButtonStatus[ButtonCount];
#ifdef USE_TOUCH
int ButtonRef[ButtonCount];
#endif

void button_init(Button button)
{
	ButtonStatus[button] = false;
#ifdef USE_TOUCH
	pinMode(ButtonPin[button], INPUT);
	ButtonRef[button] = 0;
	pinMode(BUTTON_SEND, OUTPUT_FAST);
#else
	pinMode(ButtonPin[button], INPUT_PULLUP);
#endif
}

bool button_is_pressing(Button button)
{
#ifdef USE_TOUCH
	int pin = ButtonPin[button];
	int t = 0;
	int tt = ButtonRef[button];
	for (int l=0; l<TOUCH_LOOP; l++)
	{
		digitalWrite(BUTTON_SEND, HIGH);
		for (int i=0; i<TOUCH_TIME_OUT; i++)
		{
			if (digitalRead(pin) == HIGH)
			{
				break;
			}
			//delayMicroseconds(TOUCH_US_TICK);
		}
		digitalWrite(BUTTON_SEND, LOW);
		for (int i=0; i<TOUCH_TIME_OUT; i++)
		{
			//delayMicroseconds(TOUCH_US_TICK);	
			if (digitalRead(pin) == LOW)
			{
				break;
			}
			t++;
		}
	}
	ButtonRef[button] = (tt + tt + tt + t) >> 2;
	tt = 1;
	nrf_debugInt(tt*1000LL + t);
	return t > (tt << 1);
#else
	return digitalRead(ButtonPin[button]) == LOW;
#endif
}

bool button_just_pressed(Button button)
{
	bool ok = false;
	if (ButtonStatus[button] && !button_is_pressing(button))
	{
		ok = true;
	}
	ButtonStatus[button] = button_is_pressing(button);
	return ok;
}
