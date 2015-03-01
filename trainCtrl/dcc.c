#include "dcc.h"

uint8_t compute_xor(uint8_t * rawbytes, uint8_t size)
{
  uint8_t xor = 0;
  uint8_t i;
  for (i = 0; i < size; ++i)
  {
    xor ^= rawbytes[i];
  }
  return xor;
}


