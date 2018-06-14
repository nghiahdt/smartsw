#ifndef __RAND_H
#define __RAND_H

#include "../stm8s/stm8s.h"

void randomSeed(uint16_t seed);
int16_t random(int16_t maximum);
int16_t random(int16_t minimum, int16_t maximum);

#endif
