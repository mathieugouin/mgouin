/**
 * Circular buffer implementation
 */

#include "buffer.h"

bufferStatus_t bufferInit(bufferStruct_t* iopBuf, bufferData_t *bufferStorage, bufferIndex_t size)
{
  if (!iopBuf || !bufferStorage)
    return BUFFER_ERROR;
    
  iopBuf->data = bufferStorage;
  iopBuf->size = size;
  iopBuf->putIdx = 0;
  iopBuf->getIdx = 0;
  
  return BUFFER_OK;
}

bufferStatus_t bufferGet(bufferStruct_t* iopBuf, bufferData_t *data)
{
  bufferIndex_t tmpGetIdx;
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
      return BUFFER_ERROR;
    }

    /* Extract label from buffer */
    *data = iopBuf->data[iopBuf->getIdx];

    /* Move out pointer to the next label */
    tmpGetIdx = iopBuf->getIdx + 1;

    /* If out pointer is passed the end of the buffer */
    if (tmpGetIdx >= iopBuf->size)
    {
      tmpGetIdx = 0;
    }
    
    // TBD check (i + 1) % size

    /* Update the out pointer */
    iopBuf->getIdx = tmpGetIdx;
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
  
  // TBD check (i + 1) % size

  /* If buffer is full (IN == OUT) */
  if (tmpPutIdx == iopBuf->getIdx)
  {
    status = BUFFER_FULL;
  }
  else
  {
    status = BUFFER_OK;

    /* Store the data in the buffer */
    iopBuf->data[iopBuf->putIdx] = data;
    /* Update the in pointer */
    iopBuf->putIdx = tmpPutIdx;
  }

  return status;
}


