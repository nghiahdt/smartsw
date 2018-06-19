#include "Serial.h"

#ifdef UART1

void HardwareSerial::begin(uint32_t baud) {
  uint8_t brr1, brr2;

  if (baud >= 115200) {
    brr1 = 0x08;
    brr2 = 0x0A;
  } else if (baud >= 74880) {
    brr1 = 0x0D;
    brr2 = 0x05;
  } else if (baud >= 57600) {
    brr1 = 0x11;
    brr2 = 0x05;
  } else if (baud >= 38400) {
    brr1 = 0x1A;
    brr2 = 0x00;
  } else if (baud >= 19200) {
    brr1 = 0x34;
    brr2 = 0x01;
  } else if (baud >= 9600) {
    brr1 = 0x68;
    brr2 = 0x02;
  } else if (baud >= 4800) {
    brr1 = 0xD0;
    brr2 = 0x05;
  } else if (baud >= 2400) {
    brr1 = 0xA0;
    brr2 = 0x1A;
  } else if (baud >= 1200) {
    brr1 = 0x41;
    brr2 = 0x35;
  } else { // 300
    brr1 = 0x05;
    brr2 = 0xD5;
  }

  UART1->CR1 &= (uint8_t)(~(UART1_CR1_M | UART1_CR1_PCEN | UART1_CR1_PS)); // Clear the word length bit & Clear the STOP bits & Clear the Parity Control bit (8 bit 1 stop no parity)
  UART1->BRR2 = (uint8_t)brr2; // Set the Fraction bits and MSB mantissa of UART1DIV
  UART1->BRR1 = (uint8_t)brr1; // Set the LSB mantissa of UART1DIV
  UART1->CR2 &= (uint8_t)~(UART1_CR2_TEN | UART1_CR2_REN); // Disable the Transmitter and Receiver before setting the LBCL, CPOL and CPHA bits
  UART1->CR3 &= (uint8_t)~(UART1_CR3_STOP | UART1_CR3_CPOL | UART1_CR3_CPHA | UART1_CR3_LBCL); // Clear the Clock Polarity, lock Phase, Last Bit Clock pulse
#ifdef SERIALBUFFERED
  UART1->CR2 |= (uint8_t)(UART1_CR2_TEN | UART1_CR2_REN | UART1_CR2_RIEN); // Set the Transmitter Enable bit and Set the Receiver Enable bit and Set the Receiver Interrupt Enable bit
#else
  UART1->CR2 |= (uint8_t)(UART1_CR2_TEN | UART1_CR2_REN); // Set the Transmitter Enable bit and Set the Receiver Enable bit
#endif
  UART1->CR1 &= (uint8_t)(~UART1_CR1_UARTD); // UART1 Enable

#ifdef SERIALBUFFERED
  _error = false;
  _rxbufpos = 0;
  _rxbuflen = 0;
  _txbufpos = 0;
  _txbuflen = 0;
#endif
}

void HardwareSerial::end() {
#ifdef SERIALBUFFERED
  UART1->CR2 &= (uint8_t)~(UART1_CR2_TEN | UART1_CR2_REN | UART1_CR2_TIEN | UART1_CR2_RIEN); // Disable the Transmitter and Receiver and Disable Transmitter and Receiver Interrupts
#else
  UART1->CR2 &= (uint8_t)~(UART1_CR2_TEN | UART1_CR2_REN); // Disable the Transmitter and Receiver
#endif
  UART1->CR1 |= (uint8_t)UART1_CR1_UARTD; // UART1 Disable
}

size_t HardwareSerial::available() {
#ifdef SERIALBUFFERED
  size_t result;

  disableInterrupts();
  result = _rxbuflen;
  enableInterrupts();

  return result;
#else
  if (UART1->SR & UART1_SR_RXNE)
    return 1;
  else
    return 0;
#endif
}

uint8_t HardwareSerial::peak() {
#ifdef SERIALBUFFERED
  uint8_t result;

  disableInterrupts();
  if (_rxbuflen) {
    uint8_t rxbufpos = _rxbufpos;
    result = _rxbuf[(rxbufpos - _rxbuflen + RXBUFSIZE) % RXBUFSIZE];
  } else
    result = 0;
  enableInterrupts();

  return result;
#else
  return 0;
#endif
}

uint8_t HardwareSerial::read() {
#ifdef SERIALBUFFERED
  while (! _rxbuflen); // Wait for incoming byte
  disableInterrupts();
  uint8_t rxbufpos = _rxbufpos;
  uint8_t c = _rxbuf[(rxbufpos - _rxbuflen + RXBUFSIZE) % RXBUFSIZE];
  --_rxbuflen;
  enableInterrupts();

  return c;
#else
  while (! (UART1->SR & UART1_SR_RXNE)); // Loop until the Read data register flag is SET

  return (uint8_t)UART1->DR;
#endif
}

size_t HardwareSerial::write(uint8_t c) {
#ifdef SERIALBUFFERED
  while (_txbuflen >= TXBUFSIZE); // Wait for buffer space
  disableInterrupts();
  _txbuf[_txbufpos] = c;
  if (++_txbufpos >= TXBUFSIZE)
    _txbufpos = 0;
  ++_txbuflen;
  UART1->CR2 |= UART1_CR2_TIEN;
  enableInterrupts();

  return sizeof(c);
#else
  UART1->DR = c;
  while (! (UART1->SR & UART1_SR_TXE)); // Loop until the end of transmission

  return sizeof(c);
#endif
}

void HardwareSerial::flush() {
#ifdef SERIALBUFFERED
  disableInterrupts();
  _error = false;
  _rxbufpos = 0;
  _rxbuflen = 0;
  _txbufpos = 0;
  _txbuflen = 0;
  enableInterrupts();
#else
  while (UART1->SR & UART1_SR_RXNE) // Read input stream
    if (UART1->DR); // (void)UART1->DR
#endif
}

#ifdef SERIALBUFFERED
INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17) {
  if (HardwareSerial::_txbuflen) {
    UART1->DR = HardwareSerial::_txbuf[(HardwareSerial::_txbufpos - HardwareSerial::_txbuflen + HardwareSerial::TXBUFSIZE) % HardwareSerial::TXBUFSIZE];
    --HardwareSerial::_txbuflen;
  }
  if (! HardwareSerial::_txbuflen)
    UART1->CR2 &= ~UART1_CR2_TIEN;
}

INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18) {
  HardwareSerial::_error = (UART1->SR & UART1_SR_OR) != 0;
  if (HardwareSerial::_rxbuflen < HardwareSerial::RXBUFSIZE) {
    uint8_t rxbufpos = HardwareSerial::_rxbufpos;
    HardwareSerial::_rxbuf[rxbufpos] = (uint8_t)UART1->DR;
    if (++HardwareSerial::_rxbufpos >= HardwareSerial::RXBUFSIZE)
      HardwareSerial::_rxbufpos = 0;
    ++HardwareSerial::_rxbuflen;
  } else // Buffer full, drop input byte
    if (UART1->DR); // (void)UART1->DR
}

volatile bool HardwareSerial::_error = false;
volatile uint8_t HardwareSerial::_rxbuf[RXBUFSIZE];
uint8_t HardwareSerial::_txbuf[TXBUFSIZE];
volatile uint8_t HardwareSerial::_rxbufpos = 0;
volatile uint8_t HardwareSerial::_rxbuflen = 0;
uint8_t HardwareSerial::_txbufpos = 0;
volatile uint8_t HardwareSerial::_txbuflen = 0;
#endif

HardwareSerial Serial;

#endif
