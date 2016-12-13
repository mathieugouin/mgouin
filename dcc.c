#include "dcc.h"

#define DCC_PIN   PIN_C0  // H-Bridge direction input

static bool dcc_pin_value = true;   // DCC signal pin value (High / Low)
static bool dcc_toggle_pin = true;  // If we need to toggle the DCC pin
static bool dcc_current_bit = true; // Current bit being sent by the DCC signal

// Array of bit not supported
// 1111111111 0 00000000 0 00000000 0 00000000 1
static const uint8_t dcc_bit_buffer_reset[38] = {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};

// 1111111111 0 11111111 0 00000000 0 11111111 1
static const uint8_t dcc_bit_buffer_idle[38]  = {1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1};

#define DCC_BIT_BUFFER dcc_bit_buffer_reset

static uint8_t compute_xor(const uint8_t * rawbytes, uint8_t size)
{
  uint8_t xor = 0;
  uint8_t i;
  for (i = 0; i < size; ++i)
  {
    xor ^= rawbytes[i];
  }
  return xor;
}

static bool dcc_get_next_bit(void)
{
  static uint8_t i = 37; //ARRAY_SIZE(DCC_BIT_BUFFER);
  //return true;
  //return false;
  i = (i + 1) % ARRAY_SIZE(DCC_BIT_BUFFER);
  return DCC_BIT_BUFFER[i];
}

// Test idea for ISR DCC signal generator (timer overflow ISR)
/*
 *                   1        0               
 *    Pin:   / HIGH: __       ____            
 *           \ LOW:    __         ____        
 *             ISR:  * *      * * * *         
 */
void dcc_signal_gen_isr(void)
{
  // Always drive pin first in the ISR to have no jitter on signal
  output_bit(DCC_PIN, dcc_pin_value);

  if (dcc_toggle_pin)
  {
    dcc_pin_value = !dcc_pin_value;

    if (dcc_pin_value) // back to 1 ?
    {
      dcc_current_bit = dcc_get_next_bit();
    }

    if (!dcc_current_bit)
    {
      dcc_toggle_pin = false;
    }
  }
  else
  {
    dcc_toggle_pin = true;
  }
}

