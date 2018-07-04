#ifndef _NRF_H_
#define _NRF_H_

#include <Arduino.h>

extern void nrf_begin();
void nrf_loop();
void nrf_sendStatus();
void nrf_debug(const char* text);
void nrf_debugInt(int32_t num);

#endif