#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <Arduino.h>

//#define USE_TOUCH

#ifdef USE_TOUCH
#define TOUCH_TIME_OUT	100
#define TOUCH_US_TICK	100
#define TOUCH_LOOP		10		
#endif

typedef enum ButtonType
{
	Button1 = 0,
	Button2,
	Button3,
	ButtonCount
} Button;

#ifdef USE_TOUCH
#define BUTTON_SEND PA2
#endif
#define BUTTON_PIN { PD4, PD4, PD4 }

void button_init(Button button);
bool button_is_pressing(Button button);
bool button_just_pressed(Button button);

#endif
