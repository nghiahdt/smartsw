#ifndef _NRF_H_
#define _NRF_H_

#include <Arduino.h>
#include <functional>
typedef std::function<void(const std::string&)> NrfFunc;

class NrfClass
{
	NrfFunc _func;

public:
	void begin(const NrfFunc& func);
	void loop();
	bool send(String text, bool endSend = true);
};

extern NrfClass Nrf;

#endif
