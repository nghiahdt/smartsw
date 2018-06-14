#ifndef __SERIAL_H
#define __SERIAL_H

#include <stddef.h>
#include "../stm8s/stm8s.h"
#include "Print.h"

#define SERIALBUFFERED

class HardwareSerial : public Print {
public:
  static void begin(uint32_t baud);
  static void end();
  static size_t available();
  static uint8_t peak();
  static uint8_t read();
  using Print::write;
  size_t write(uint8_t c);
  static void flush();
#ifdef SERIALBUFFERED
  static bool error() {
    return _error;
  }
protected:
  static const uint8_t RXBUFSIZE = 16, TXBUFSIZE = 16;
  static volatile bool _error;
  static volatile uint8_t _rxbuf[RXBUFSIZE];
  static uint8_t _txbuf[TXBUFSIZE];
  static volatile uint8_t _rxbufpos, _rxbuflen;
  static uint8_t _txbufpos;
  static volatile uint8_t _txbuflen;

  friend __interrupt void UART1_TX_IRQHandler();
  friend __interrupt void UART1_RX_IRQHandler();
#endif
};

extern HardwareSerial Serial;

#endif
