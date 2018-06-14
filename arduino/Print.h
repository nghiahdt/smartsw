#ifndef __PRINT_H
#define __PRINT_H

#include <stddef.h>
#include "../stm8s/stm8s.h"
#include "Printable.h"

const uint8_t BIN = 2, OCT = 8, DEC = 10, HEX = 16;

class Print {
public:
  virtual size_t write(uint8_t c) = 0;
  virtual size_t write(const uint8_t* buf, size_t bufsize);
  size_t print(char c);
  size_t print(const char* str);
  size_t print(uint8_t n, uint8_t notation = DEC);
  size_t print(int16_t n, uint8_t notation = DEC);
  size_t print(uint16_t n, uint8_t notation = DEC);
  size_t print(int32_t n, uint8_t notation = DEC);
  size_t print(uint32_t n, uint8_t notation = DEC);
  size_t print(float n, uint8_t prec = 2);
  size_t print(const Printable& p);
  size_t println();
  size_t println(char c);
  size_t println(const char* str);
  size_t println(uint8_t n, uint8_t notation = DEC);
  size_t println(int16_t n, uint8_t notation = DEC);
  size_t println(uint16_t n, uint8_t notation = DEC);
  size_t println(int32_t n, uint8_t notation = DEC);
  size_t println(uint32_t n, uint8_t notation = DEC);
  size_t println(float n, uint8_t prec = 2);
  size_t println(const Printable& p);
};

#endif
