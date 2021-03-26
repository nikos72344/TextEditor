#include "cursor.h"
#include <stdio.h>

void cursorInit(cursor* cursor, metrics* metrics, HWND hwnd, storageModel* buffer, viewModel* actual, int* firstString) {
  cursor->storagePos = 0;
  cursor->stringPos = 0;
  CreateCaret(hwnd, NULL, 2, metrics->charY);
  setCursorPos(cursor, metrics, buffer, actual, firstString);
}

int getStrLen(cursor* cursor, storageModel* buffer, viewModel* actual)
{
  int len = (cursor->stringPos == actual->strCount - 1) ? buffer->size - actual->strBegIndices[cursor->stringPos] - 1 : actual->strBegIndices[cursor->stringPos + 1] - actual->strBegIndices[cursor->stringPos];
  if (buffer->buffer[max(0, actual->strBegIndices[cursor->stringPos + 1] - 1)] == '\n')
    len--;
  if (buffer->buffer[max(0, actual->strBegIndices[cursor->stringPos + 1] - 1)] == '\0')
    len--;
  if (buffer->buffer[max(0, actual->strBegIndices[cursor->stringPos + 1] - 2)] == '\r')
    len--;
  return max(len, 0);
}

void cursorLimit(cursor* cursor, storageModel* buffer, viewModel* actual, int* firstString) {
  cursor->stringPos = max(min(max(0, cursor->stringPos), actual->strCount - 1), 0);
  int len = getStrLen(cursor, buffer, actual);
  cursor->storagePos = max(0, min(cursor->storagePos, actual->strBegIndices[cursor->stringPos] + len));
  printf("%i %i\n", cursor->storagePos, actual->strBegIndices[cursor->stringPos]);
}

void setCursorPos(cursor* cursor, metrics* metrics, storageModel* buffer, viewModel* actual, int* firstString) {
  cursorLimit(cursor, buffer, actual, firstString);
  printf("[%i]\n", max(0, cursor->storagePos - actual->strBegIndices[cursor->stringPos]) + 1);
  SetCaretPos(metrics->charX * (max(0, cursor->storagePos - actual->strBegIndices[cursor->stringPos]) + 1), metrics->charY * (cursor->stringPos - *firstString));
}

void cursorReset(cursor* cursor, metrics* metrics, storageModel* buffer, viewModel* actual, int* firstString) {
  cursor->storagePos = 0;
  cursor->stringPos = 0;
  setCursorPos(cursor, metrics, buffer, actual, firstString);
}

void horzLimit(cursor* cursor, viewModel* actual, storageModel* buffer) {
  int len = getStrLen(cursor, buffer, actual);
  if (cursor->storagePos - actual->strBegIndices[cursor->stringPos] > len)
    cursor->storagePos = len + actual->strBegIndices[cursor->stringPos];
  if (cursor->storagePos < actual->strBegIndices[cursor->stringPos])
    cursor->storagePos = actual->strBegIndices[cursor->stringPos];
}

void cursorMove(cursor* cursor, viewModel* actual, storageModel* buffer, metrics* metrics, WPARAM wParam, int* firstString) {
  int inc = cursor->storagePos - actual->strBegIndices[cursor->stringPos];
  switch (wParam) {
  case VK_LEFT:
    cursor->storagePos--;
    break;

  case VK_RIGHT:
    cursor->storagePos++;
    break;

  case VK_UP:
    cursor->stringPos = max(cursor->stringPos - 1, 0);
    cursor->storagePos = actual->strBegIndices[cursor->stringPos] + inc;
    break;

  case VK_DOWN:
    cursor->stringPos = min(cursor->stringPos + 1, actual->strCount - 1);
    cursor->storagePos = actual->strBegIndices[cursor->stringPos] + inc;
    break;

  }

  horzLimit(cursor, actual, buffer);
  setCursorPos(cursor, metrics, buffer, actual, firstString);
}