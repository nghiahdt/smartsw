#include "relay.h"
#include "button.h"
#include "nrf.h"

void setup()
{
  relay_init(Relay1);
  //button_init(Button1);
  nrf_begin();
}

void loop()
{
  uint8_t data[NRF_PAYLOAD_LENGTH] = { 'f', 'u', 'c', 'k', 0};

  //if (button_just_pressed(Button1))
  {
    relay_change(Relay1);
  }

  if (nrf24_getStatus() == 255)
  {
    relay_on(Relay1);
  }

  nrf24_send(data);
  while(nrf24_isSending());

  delay(100);
}

