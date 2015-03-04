/**
 * Circular buffer implementation
 */


#ifndef BUFFER_H
#define BUFFER_H

#include "common.h"

typedef uint8_t bufferData_t;
typedef uint8_t bufferIndex_t;

typedef enum
{
  BUFFER_EMPTY,
  BUFFER_FULL,
  BUFFER_ERROR,
  BUFFER_OK
} bufferStatus_t;

typedef struct
{
  bufferData_t *data;   // ptr to data storage
  bufferIndex_t size;   // buffer size (related to *data)
  bufferIndex_t putIdx; // put => write index
  bufferIndex_t getIdx; // get => read index
} bufferStruct_t;

bufferStatus_t bufferInit(bufferStruct_t* iopBuf, bufferData_t *bufferStorage, bufferIndex_t size);
bufferStatus_t bufferGet(bufferStruct_t* iopBuf, bufferData_t *data);
bufferStatus_t bufferPut(bufferStruct_t* iopBuf, bufferData_t data);

#endif // BUFFER_H
