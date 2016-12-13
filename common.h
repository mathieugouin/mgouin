/*************************************************************************
* Compiler :  CCS C - 16F876
* Description : Common typedefs & macros
*************************************************************************/

#ifndef COMMON_H
#define COMMON_H

#define false                   0
#define true                    1

/* 1 bit */
typedef unsigned short int  bool;

/* 8 bits */
typedef signed              int8_t;
typedef unsigned int        uint8_t;

/* 16 bits */
typedef signed long int     int16_t;
typedef unsigned long int   uint16_t;

/* 32 bits - not supported */

#define ARRAY_SIZE(a)  (sizeof(a) / sizeof(a[0]))

#define ABS(x)  ((x) < 0 ? -(x) : (x))
// TBD slow...
// #define SIGN(x) (((x) > 0) - ((x) < 0))


#endif /* COMMON_H */
