#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <Arduino.h>

typedef enum ButtonType
{
	Button1 = 0,
	Button2,
	Button3,
	Button4,
	ButtonCount
} Button;

#define BUTTON_PIN { PC3, PC4, PC5, PC6 }
#define BUTTON_ACTIVE HIGH

void button_init(Button button);
bool button_is_pressing(Button button);
bool button_just_pressed(Button button);

#endif
