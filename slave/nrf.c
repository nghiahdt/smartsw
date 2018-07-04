#include "nrf.h"
#include "func.h"
#include "relay.h"

#define STM8S_MCU
#define NRF_CE_PIN			PA2
#define NRF_CSN_PIN			PA1
#define NRF_SCK_PIN			PD6	
#define NRF_MOSI_PIN		PD5
#define NRF_MISO_PIN		PD4
#include <nrf24sw.h>
#include <jsmn.h>

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start && strncmp(json + tok->start, s, tok->end - tok->start) == 0)
	{
		return 0;
	}
	return -1;
}

void nrf_loop()
{
	if (nrf24_dataReady())
	{
		int r, i;
		uint16_t x, m, c;
		const char* data = nrf24_getString();
		jsmn_parser parser;
		jsmntok_t tokens[8];
		jsmn_init(&parser);
		r =  jsmn_parse(&parser, data, strlen(data), tokens, sizeof(tokens)/sizeof(tokens[0]));
		for (i = 1; i < r; i++)
		{
			#define GET_VALUE(KEY, VAR) if (jsoneq(data, &tokens[i], KEY) == 0) { \
				char s[8]; \
				int l = tokens[i+1].end-tokens[i+1].start; \
				memcpy( s, data + tokens[i+1].start, l ); \
				s[l] = '\0'; \
				VAR = atoi(s); \
				i++; }
			GET_VALUE("x", x);
			GET_VALUE("m", m);
			GET_VALUE("c", c);
		}
		if (x == getId())
		{
			setMasterId(m);
			#define CONTROL_RELAY(X, R) { \
				int v = (c / X) % 10; \
				if (v == 1) relay_on(R); \
				if (v == 0) relay_off(R); }
			CONTROL_RELAY(  1, Relay1);
			CONTROL_RELAY( 10, Relay2);
			CONTROL_RELAY(100, Relay3);
		}
	}
}

void nrf_sendStatus()
{
	//{"x":12345,"m":54321,"s":12345}
	uint16_t status = 0;
	char buff[32];
	status += relay_is_on(Relay1) ?   1 : 0;
	status += relay_is_on(Relay2) ?  10 : 0;
	status += relay_is_on(Relay3) ? 100 : 0;
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
	itoa32(num, buff, 16, 10);
	nrf_debug(buff);
}