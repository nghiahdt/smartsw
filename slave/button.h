#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <Arduino.h>

#define USE_TOUCH

#ifdef USE_TOUCH
#define TOUCH_TIME_OUT	1000
#define TOUCH_US_TICK	1
#define TOUCH_LOOP		10		
#endif

typedef enum ButtonType
{
	Button1 = 0,
	Button2,
	Button3,
	Button4,
	Button5,
	ButtonCount
} Button;

#ifdef USE_TOUCH
#define BUTTON_SEND PA3
#endif
#define BUTTON_PIN { PC6, PC7, PD1, PD2, PD3 }

void button_init(Button button);
bool button_is_pressing(Button button);
bool button_just_pressed(Button button);

#endif
