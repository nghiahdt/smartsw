#ifndef _FUNC_H_
#define _FUNC_H_

#include <Arduino.h>

uint16_t getId();
const char* getIdString();
void setMasterId(const char* id);
const char* getMasterId();

const char* intToString(uint16_t num);
void uitoa(uint16_t value, char* string, int length, char radix);
void itoa(int16_t value, char* string, int length, char radix);

#endif