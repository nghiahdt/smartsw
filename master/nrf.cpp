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
	_working = false;
	os_timer_setfn(&_timer, [](void* _this) {
		if (!((NrfClass*)_this)->_working)
		{
			((NrfClass*)_this)->_working = true;
			std::string text = nrf24_getString();
			if (!text.empty())
			{
				((NrfClass*)_this)->_func(text);
			}
			((NrfClass*)_this)->_working = false;
		}
	}, this);
	os_timer_arm(&_timer, 100, true);
}

bool NrfClass::send(String text, bool endSend)
{
	bool ok = false;
	if (!_working)
	{
		_working = true;
		ok = nrf24_sendString(text, endSend);
		_working = false;
	}
	return ok;
}