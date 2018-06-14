#ifndef __PRINTABLE_H
#define __PRINTABLE_H

#include <stddef.h>

class Print;

class Printable {
public:
  virtual size_t printTo(Print& p) const = 0;
};

#endif
