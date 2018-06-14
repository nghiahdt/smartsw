#include "EEPROM.h"

bool _EEPROM::begin(bool readonly) {
  FLASH->CR1 &= (uint8_t)(~FLASH_CR1_FIX);

  if (! readonly) {
    FLASH->DUKR = FLASH_RASS_KEY2;
    FLASH->DUKR = FLASH_RASS_KEY1;

    return (FLASH->IAPSR & FLASH_IAPSR_DUL) != 0;
  } else
    return true;
}

void _EEPROM::end() {
  FLASH->IAPSR &= (uint8_t)(~FLASH_IAPSR_DUL);
}

uint8_t _EEPROM::read(uint16_t address) {
  if (address < length())
    return *(PointerAttr uint8_t*)(MemoryAddressCast)(FLASH_DATA_START_PHYSICAL_ADDRESS + address);
}

void _EEPROM::write(uint16_t address, uint8_t data) {
  if (address < length())
    *(PointerAttr uint8_t*)(MemoryAddressCast)(FLASH_DATA_START_PHYSICAL_ADDRESS + address) = data;
}

uint8_t& _EEPROM::byteAt(uint16_t address) {
  if (address < length())
    return *(PointerAttr uint8_t*)(MemoryAddressCast)(FLASH_DATA_START_PHYSICAL_ADDRESS + address);
}

_EEPROM EEPROM;
