#include "Rand.h"
#include <stdlib.h>

void randomSeed(uint16_t seed) {
  if (seed)
    srand(seed);
}

int16_t random(int16_t maximum) {
  if (! maximum)
    return 0;

  return (rand() % maximum);
}

int16_t random(int16_t minimum, int16_t maximum) {
  if (minimum >= maximum)
    return minimum;

  return (random(maximum - minimum) + minimum);
}
