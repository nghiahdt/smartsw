#ifndef __EEPROM_H
#define __EEPROM_H

#include "../stm8s/stm8s.h"

#undef EEPROM

class _EEPROM {
public:
  static bool begin(bool readonly = false);
  static void end();
  static uint16_t length() {
    return (FLASH_DATA_END_PHYSICAL_ADDRESS - FLASH_DATA_START_PHYSICAL_ADDRESS + 1);
  }
  static uint8_t read(uint16_t address);
  static void write(uint16_t address, uint8_t data);
  static uint8_t& byteAt(uint16_t address);
  uint8_t& operator[](uint16_t address) {
    return byteAt(address);
  }
  template<typename T> static T& get(uint16_t address, T& t) {
    if (address + sizeof(T) <= length()) {
      uint8_t* ptrt = (uint8_t*)&t;

      for (uint16_t i = 0; i < sizeof(T); ++i) {
        *ptrt++ = *(PointerAttr uint8_t*)(MemoryAddressCast)(FLASH_DATA_START_PHYSICAL_ADDRESS + address++);
      }

      return t;
    }
    return 0;
  }
  template<typename T> static const T& put(uint16_t address, const T& t) {
    if (address + sizeof(T) <= length()) {
      uint8_t* ptrt = (uint8_t*)&t;

      for (uint16_t i = 0; i < sizeof(T); ++i) {
        *(PointerAttr uint8_t*)(MemoryAddressCast)(FLASH_DATA_START_PHYSICAL_ADDRESS + address++) = *ptrt++;
      }

      return t;
    }
    return 0;
  }
};

extern _EEPROM EEPROM;

#endif
