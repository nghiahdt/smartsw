#include "relay.h"
#include "button.h"
#include "nrf.h"
#include "led.h"

// STM8S103F3            +-\/-+
// PWM      (D 13) PD4  1|    |20  PD3 (D 12, AI 2) PWM
// TX (D 14, AI 3) PD5  2|    |19  PD2 (D 11, AI 1)
// RX (D 15, AI 4) PD6  3|    |18  PD1 (D 10)  <SWIM>
//                NRST  4|    |17  PC7 (D 9)   MISO/PWM+
//           (D 0) PA1  5|    |16  PC6 (D 8)   MOSI/PWM+
//           (D 1) PA2  6|    |15  PC5 (D 7)    SCK
//                 GND  7|    |14  PC4 (D 6, AI 0)  PWM
//                Vcap  8|    |13  PC3 (D 5)        PWM
//                 Vdd  9|    |12  PB4 (D 4)        SCL (T)
// PWM       (D 2) PA3 10|    |11  PB5 (D 3)   LED, SDA (T)
// PWM+ indicates the    +----+    additional PWM pins on alternate function pins

// Status: PB5 //LED_BUILTIN
// Relay:  PC7, PD2, PD3
// Led:    PA3, PB4, PD1
// Button: PC3, PC4, PC5, PC6
// Nrf:    CE-PA2, CSN-PA1, SCK-PD6, MOSI-PD5, MISO-PD4

#define SEND_STATUS_TIME 100

void setup()
{
	relay_init(Relay1);
	relay_init(Relay2);
	relay_init(Relay3);
	led_init(Led1);
	led_init(Led2);
	led_init(Led3);
	led_init(LedStatus);
	button_init(Button1);
	button_init(Button2);
	button_init(Button3);
	nrf_begin();
}

void loop()
{
	extern void updateStatus(int dt);
	extern void sendStatus(int dt);
	int dt = getDt();
	nrf_loop();
	updateStatus(dt);
	sendStatus(dt);
}

int getDt()
{
	static uint32_t lastTime = 0;
	uint32_t nowTime = millis();
	int dt = nowTime - lastTime;
	lastTime = nowTime;
	return dt;
}

void update_relay_led(Relay relay, Led led)
{
	if (relay_is_on(relay))
	{
		led_on(led);
	}
	else
	{
		led_off(led);
	}
}

void updateStatus(int dt)
{
	if (button_just_pressed(Button1))
	{
		relay_change(Relay1);
	}
	if (button_just_pressed(Button2))
	{
		relay_change(Relay2);
	}
	if (button_just_pressed(Button3))
	{
		relay_change(Relay3);
	}
	if (button_is_pressing(Button1))
	{
		led_blink(Led1, dt);
	}
	else
	{
		update_relay_led(Relay1, Led1);
	}
	if (button_is_pressing(Button2))
	{
		led_blink(Led2, dt);
	}
	else
	{
		update_relay_led(Relay2, Led2);
	}
	if (button_is_pressing(Button3))
	{
		led_blink(Led3, dt);
	}
	else
	{
		update_relay_led(Relay3, Led3);
	}
	led_blink(LedStatus, dt);
}

void sendStatus(int dt)
{
	static int c = 0;
	c += dt;
	if (c >= SEND_STATUS_TIME)
	{
		c = 0;
		nrf_sendStatus();
	}
}