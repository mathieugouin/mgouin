#include "common.h"

#define BUFFER_SIZE    32

typedef uint8_t bufferData_t;
typedef uint8_t bufferIndex_t;

// TBD Temp
bufferData_t data[BUFFER_SIZE];

typedef enum
{
  BUFFER_EMPTY,
  BUFFER_FULL,
  BUFFER_OK
} bufferStatus_t;

typedef struct
{
  bufferData_t *data;
  bufferIndex_t size;
  bufferIndex_t putIdx;
  bufferIndex_t getIdx;
} bufferStruct_t;

void bufferInit(bufferStruct_t* iopBuf, bufferData_t *data, bufferIndex_t size)
{
  iopBuf->data = data;
  iopBuf->size = size;
  iopBuf->putIdx = 0;
  iopBuf->getIdx = 0;
}

bufferStatus_t bufferGet(bufferStruct_t* iopBuf, bufferData_t *data)
{
  bufferIndex_t TmpGetIdx;
  bufferStatus_t status;

  /* If buffer is empty (IN == OUT) */
  if (iopBuf->putIdx == iopBuf->getIdx)
  {
    status = BUFFER_EMPTY;
  }
  else
  {
    status = BUFFER_OK;

    /* Memory corruption verification */
    if (iopBuf->getIdx >= iopBuf->size)
    {
      // TBD FAILURE
    }

    /* Extract label from buffer */
    *data = iopBuf->data[iopBuf->getIdx];

    /* Move out pointer to the next label */
    TmpGetIdx = iopBuf->getIdx + 1;

    /* If out pointer is passed the end of the buffer */
    if (TmpGetIdx >= iopBuf->size)
    {
      TmpGetIdx = 0;
    }

    /* Update the out pointer */
    iopBuf->getIdx = TmpGetIdx;
  }

  return status;
}

bufferStatus_t bufferPut(bufferStruct_t* iopBuf, bufferData_t data)
{
  bufferIndex_t tmpPutIdx;
  bufferStatus_t status;

  /* Move in pointer to the next label */
  tmpPutIdx = iopBuf->putIdx + 1;

  /* If in pointer is passed the end of the buffer */
  if (tmpPutIdx >= iopBuf->size)
  {
    /* Set in pointer to the start of the buffer */
    tmpPutIdx = 0;
  }

  /* If buffer is full (IN == OUT) */
  if (tmpPutIdx == iopBuf->getIdx)
  {
    status = BUFFER_FULL;
  }
  else
  {
    status = BUFFER_OK;

    /* Store the label in the buffer */
    iopBuf->data[iopBuf->putIdx] = data;
    /* Update the in pointer */
    iopBuf->putIdx = tmpPutIdx;
  }

  return status;
}

