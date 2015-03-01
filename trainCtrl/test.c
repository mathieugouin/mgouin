#include "common.h"
#include <string.h>

#include "dcc.c"

// RAM fill test

typedef struct
{
  uint16_t x;
  uint8_t y;
} TestStruct_t;

static TestStruct_t s;
TestStruct_t * ps = &s;

/*
static uint8_t tab[79];
static uint8_t tab2[35];
static uint8_t tab3[1];
*/

void f1(TestStruct_t * ps)
{
  ps->x++;
  ps->y--;
}

void f3(char * str, int size)
{
  int i = 0;
  while (i < size)
  {
    str[i]++;
    i++;
  }
}

void f2(void)
{
  char str[5] = "ALLO";
  TestStruct_t ss;
  f1(ps);
  f1(&ss);
  f3(str, strlen(str));
  f3(str, 4);
}

void main_test(void)
{
  char str[5] = "ALLO";
  compute_xor(str, 4);
  f2();
}

