#include "nrf.h"

#define NRF_CE_PIN			D0
#define NRF_CSN_PIN			D1
#define NRF_SCK_PIN			D2	
#define NRF_MOSI_PIN		D3
#define NRF_MISO_PIN		D7
#include <nrf24sw.h>

NrfClass Nrf;

void NrfClass::begin(const NrfFunc& func)
{
	nrf_begin();
	_func = func;
	os_timer_setfn(&_timer, [](void* _this) {
		std::string text = nrf24_getString();
		if (!text.empty())
		{
			((NrfClass*)_this)->_func(text);
		}
	}, this);
	os_timer_arm(&_timer, 100, true);
}

bool NrfClass::send(String text, bool endSend)
{
	return nrf24_sendString(text, endSend);
}