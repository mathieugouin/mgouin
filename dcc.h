/**
 * Nominally each half cycle of a "1" bit must have a period of 58 microseconds
 * giving a total time of 116 microseconds. To allow some design latitude a
 * command station may transmit 1's with a period from 55 to 61 microseconds
 * long and a receiver must accept as a "1" a half cycle from 52 to 64
 * microseconds long.
 *
 * As previously mentioned each half cycle of a "0" must be roughly twice that
 * of a "1", at least 100 microseconds.
 *
 * http://members.iinet.net.au/~backway/OzDcc/dcctut02.htm
 * 1111111111 0 0AAAAAAA 0 01DUSSSS 0 EEEEEEEE 1
 *
 * Preamble       Address       Data 1         Data 2          Error           Packet end bit
 * (12 1's)       0-127           up to 5 Data bytes           XOR of all 
 *                                                             Address & Data
 * 111111111111   0 AAAAAAAA    0 DDDDDDDD     0 DDDDDDDD      0 EEEEEEEE      1
 *
 *
 * Reset Packet
 * 1111111111 0 00000000 0 00000000 0 00000000 1
 *
 * Idle Packet
 * 1111111111 0 11111111 0 00000000 0 11111111 1
 *
 * For DCC:
 *  - Timer1
 *  - CCP1
 *  - Special event => RST Timer1
 *
 * Ftimer = Fc / 4 / Prescaler (1,2,4,8)
 * Fccp = Ftimer / (CCP + 1)
 *
 * => Prescaler = 1
 * => CCP = 289
 *
 * When tuning CCP, this gives 200ns of delta tuning
 */


#ifndef DCC_H
#define DCC_H

#include "common.h"


#endif // DCC_H
