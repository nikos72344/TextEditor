#include "editor.h"
#include <stdio.h>

BOOLEAN addChar(storageModel* buffer, cursor* cursor, char ch) {
  int i = 0, flag = 0;
  if (ch == '\b')
    return FALSE;
  char* newBuffer = (char*)malloc((buffer->size + 1) * sizeof(char));
  if (newBuffer == NULL)
    return FALSE;
  for (i = 0; i < buffer->size; i++) {
    if (i == cursor->storagePos) {
      newBuffer[i] = ch;
      flag = 1;
    }
    newBuffer[i + flag] = buffer->buffer[i];
  }
  //free(buffer->buffer);
  buffer->buffer = newBuffer;
  buffer->size++;
  return TRUE;
}

void addReturn(storageModel* buffer, cursor* cursor) {
  int i = 0, flag = 0;
  char* newBuffer = (char*)malloc((buffer->size + 2) * sizeof(char));
  if (newBuffer == NULL)
    return;
  for (i = 0; i < buffer->size; i++) {
    if (i == cursor->storagePos) {
      newBuffer[i] = '\r';
      newBuffer[i + 1] = '\n';
      flag = 2;
    }
    newBuffer[i + flag] = buffer->buffer[i];
  }
  //free(buffer->buffer);
  buffer->buffer = newBuffer;
  buffer->size += 2;
}

BOOLEAN deleteChar(storageModel* buffer, cursor* cursor) {
  int i = 0, flag = 0;
  if (buffer->size <= 1 || cursor->storagePos == 0)
    return FALSE;
  char* newBuffer = (char*)malloc((buffer->size - 1) * sizeof(char));
  if (newBuffer == NULL)
    return FALSE;
  for (i = 0; i < buffer->size; i++) {
    if (i == cursor->storagePos)
      flag = 1;
    newBuffer[i - flag] = buffer->buffer[i];
  }
  //free(buffer->buffer);
  buffer->buffer = newBuffer;
  buffer->size--;
  return TRUE;
}