#include "nrf.h"
#include "func.h"
#include "relay.h"

#define STM8S_MCU
#define NRF_CE_PIN			PD4
#define NRF_CSN_PIN			PD5
#define NRF_SCK_PIN			PD6	
#define NRF_MOSI_PIN		PD1
#define NRF_MISO_PIN		PD2
#include <nrf24sw.h>
#include <jsmn.h>

void nrf_loop()
{
	if (nrf24_dataReady())
	{
		const char* data = nrf24_getString();
		//{"x":12345,"m":54321,"c":12345}
		uint16_t x, m, c;
		jsmn_parser p;
		jsmn_init(&p);
		//.......
		if (x == getId())
		{
			setMasterId(m);
			#define CONTROL_RELAY(X, R) { \
				int v = (c / X) % 10; \
				if (v == 1) relay_on(R); \
				if (v == 0) relay_off(R); }
			CONTROL_RELAY(100, Relay1);
			CONTROL_RELAY( 10, Relay2);
			CONTROL_RELAY(  1, Relay3);
		}
	}
}

void nrf_sendStatus()
{
	//{"x":12345,"m":54321,"s":12345}
	uint16_t status = 0;
	char buff[32];
	status += relay_is_on(Relay1) ? 100 : 0;
	status += relay_is_on(Relay2) ?  10 : 0;
	status += relay_is_on(Relay3) ?   1 : 0;
	strcpy(buff, "{\"x\":");
	strcat(buff, getIdString());
	strcat(buff, ",\"m\":\"");
	strcat(buff, getMasterIdString());
	strcat(buff, ",\"s\":");
	strcat(buff, intToString(status));
	strcat(buff, "}");
	nrf24_sendString(buff);
}

void nrf_debug(const char* text)
{
	//{"x":12345,"d":"1234567890123"}
	char buff[32];
	strcpy(buff, "{\"x\":");
	strcat(buff, getIdString());
	strcat(buff, ",\"d\":\"");
	strcat(buff, text);
	strcat(buff, "\"}");
	nrf24_sendString(buff);
}

void nrf_debugInt(int32_t num)
{
	char buff[16];
	itoa(num, buff, 16, 10);
	nrf_debug(buff);
}