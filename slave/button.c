#include "button.h"
#include "nrf.h"

int ButtonPin[ButtonCount] = BUTTON_PIN;
bool ButtonStatus[ButtonCount];

void button_init(Button button)
{
	ButtonStatus[button] = false;
	pinMode(ButtonPin[button], INPUT_PULLUP);
}

bool button_is_pressing(Button button)
{
	return digitalRead(ButtonPin[button]) == BUTTON_ACTIVE;
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
