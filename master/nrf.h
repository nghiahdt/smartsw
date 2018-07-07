#ifndef _NRF_H_
#define _NRF_H_

#include <Arduino.h>
#include <functional>
typedef std::function<void(const std::string&)> NrfFunc;
extern "C" {
#include "user_interface.h"
}

class NrfClass
{
	NrfFunc _func;
	os_timer_t _timer;
	bool _working;

public:
	void begin(const NrfFunc& func);
	bool send(String text, bool endSend = true);
};

extern NrfClass Nrf;

#endif
