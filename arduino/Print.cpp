#include <math.h>
#include "Print.h"

size_t Print::write(const uint8_t* buf, size_t bufsize) {
  size_t ret = 0;

  while (bufsize--) {
    if (write(*buf++))
      ++ret;
    else
      break;
  }

  return ret;
}

size_t Print::print(char c) {
  return write(c);
}

size_t Print::print(const char* str) {
  size_t ret = 0;

  if (str == NULL)
    return 0;
  while (*str) {
    if (write(*str++))
      ++ret;
    else
      break;
  }

  return ret;
}

size_t Print::print(uint8_t n, uint8_t notation) {
  return print((uint16_t)n, notation);
}

size_t Print::print(int16_t n, uint8_t notation) {
  if (n < 0) {
    if (! write('-'))
      return 0;
    return (print((uint16_t)(-n), notation) + 1);
  } else {
    return print((uint16_t)n, notation);
  }
}

size_t Print::print(uint16_t n, uint8_t notation) {
  size_t ret = 0;
  uint16_t divider;
  uint8_t digit;

  if (notation == BIN)
    divider = (uint16_t)1 << 15;
  else if (notation == OCT)
    divider = 0100000;
  else if (notation == HEX)
    divider = 0x1000;
  else // DEC
    divider = 10000;
  for (; divider > 0; divider /= notation) {
    digit = (n / divider) % notation;
    if (digit || (divider == 1) || (n > divider)) {
      if (! write(digit + ((digit < 10) ? '0' : ('A' - 10))))
        break;
      ++ret;
    }
  }

  return ret;
}

size_t Print::print(int32_t n, uint8_t notation) {
  if (n < 0) {
    if (! write('-'))
      return 0;
    return (print((uint32_t)(-n), notation) + 1);
  } else {
    return print((uint32_t)n, notation);
  }
}

size_t Print::print(uint32_t n, uint8_t notation) {
  size_t ret = 0;
  uint32_t divider;
  uint8_t digit;

  if (notation == BIN)
    divider = (uint32_t)1 << 31;
  else if (notation == OCT)
    divider = 010000000000;
  else if (notation == HEX)
    divider = 0x10000000;
  else // DEC
    divider = 1000000000;
  for (; divider > 0; divider /= notation) {
    digit = (n / divider) % notation;
    if (digit || (divider == 1) || (n > divider)) {
      if (! write(digit + ((digit < 10) ? '0' : ('A' - 10))))
        break;
      ++ret;
    }
  }

  return ret;
}

size_t Print::print(float n, uint8_t prec) {
  size_t ret = 0;

  if (isnan(n))
    return print("nan");
  if (isinf(n))
    return print("inf");
  if ((n < -4294967040.0f) || (n > 4294967040.0f)) // Constants determined empirically
    return print("ovf");

  if (n < 0.0f) {
    if (! write('-'))
      return 0;
    ++ret;
    n = -n;
  }
  // Round correctly so that print(1.999, 2) prints as "2.00"
  float r = 0.5f;
  for (uint8_t i = 0; i < prec; ++i)
    r /= 10.0f;
  n += r;
  // Extract the integer part of the number and print it
  uint32_t intpart = (uint32_t)n;
  r = n - (float)intpart;
  ret += print(intpart);
  // Print the decimal point, but only if there are digits beyond
  if (prec > 0) {
    ret += print('.');
    // Extract digits from the remainder one at a time
    while (prec--) {
      r *= 10.0f;
      uint8_t digit = (uint8_t)r;
      ret += print(digit);
      r -= digit;
    }
  }

  return ret;
}

size_t Print::print(const Printable& p) {
  return p.printTo(*this);
}

size_t Print::println() {
  size_t ret = write('\r');

  return (write('\n') + ret);
}

size_t Print::println(char c) {
  size_t ret = print(c);

  return (println() + ret);
}

size_t Print::println(const char* str) {
  size_t ret = print(str);

  return (println() + ret);
}

size_t Print::println(uint8_t n, uint8_t notation) {
  size_t ret = print(n, notation);

  return (println() + ret);
}

size_t Print::println(int16_t n, uint8_t notation) {
  size_t ret = print(n, notation);

  return (println() + ret);
}

size_t Print::println(uint16_t n, uint8_t notation) {
  size_t ret = print(n, notation);

  return (println() + ret);
}

size_t Print::println(int32_t n, uint8_t notation) {
  size_t ret = print(n, notation);

  return (println() + ret);
}

size_t Print::println(uint32_t n, uint8_t notation) {
  size_t ret = print(n, notation);

  return (println() + ret);
}

size_t Print::println(float n, uint8_t prec) {
  size_t ret = print(n, prec);

  return (println() + ret);
}

size_t Print::println(const Printable& p) {
  size_t ret = print(p);

  return (println() + ret);
}
