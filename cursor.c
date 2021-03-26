#include "cursor.h"
#include "functions.h"
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

void horzLimit(cursor* cursor, viewModel* actual, storageModel* buffer) {
  int len = getStrLen(cursor, buffer, actual);
  if (cursor->storagePos - actual->strBegIndices[cursor->stringPos] > len)
    cursor->storagePos = len + actual->strBegIndices[cursor->stringPos];
  if (cursor->storagePos < actual->strBegIndices[cursor->stringPos])
    cursor->storagePos = actual->strBegIndices[cursor->stringPos];
}

void keyHScroll(cursor* cursor, viewModel* actual, storageModel* buffer, metrics* metrics) {
  if (cursor->storagePos - actual->strBegIndices[cursor->stringPos] - metrics->HscrollPos + 1 > metrics->clientX / metrics->charX)
    metrics->HscrollPos = metrics->HscrollPos + 1;
  if (max(0, cursor->storagePos - actual->strBegIndices[cursor->stringPos]) < metrics->HscrollPos)
    metrics->HscrollPos = max(metrics->HscrollPos - 1, 0);
}

void cursorLimit(cursor* cursor, storageModel* buffer, viewModel* actual, int* firstString, metrics* metrics) {
  cursor->stringPos = max(min(max(0, cursor->stringPos), actual->strCount - 1), 0);
  int len = getStrLen(cursor, buffer, actual);
  cursor->storagePos = max(0, min(cursor->storagePos, actual->strBegIndices[cursor->stringPos] + len));
  horzLimit(cursor, actual, buffer);
  keyHScroll(cursor, actual, buffer, metrics);
  //printf("%i %i\n", cursor->storagePos, actual->strBegIndices[cursor->stringPos]);
}

void setCursorPos(cursor* cursor, metrics* metrics, storageModel* buffer, viewModel* actual, int* firstString) {
  cursorLimit(cursor, buffer, actual, firstString, metrics);
  //printf("[%i]\n", max(0, cursor->storagePos - actual->strBegIndices[cursor->stringPos]) + 1);
  SetCaretPos(metrics->charX * (max(0, cursor->storagePos - actual->strBegIndices[cursor->stringPos]) + 1 - metrics->HscrollPos), metrics->charY * (cursor->stringPos - *firstString));
}

void cursorReset(cursor* cursor, metrics* metrics, storageModel* buffer, viewModel* actual, int* firstString) {
  cursor->storagePos = 0;
  cursor->stringPos = 0;
  setCursorPos(cursor, metrics, buffer, actual, firstString);
}

void cursorMove(cursor* cursor, viewModel* actual, storageModel* buffer, metrics* metrics, WPARAM wParam, int* firstString, HWND hwnd) {
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
    if (cursor->stringPos < *firstString)
      *firstString = (*firstString) - 1;
    break;

  case VK_DOWN:
    cursor->stringPos = min(cursor->stringPos + 1, actual->strCount - 1);
    cursor->storagePos = actual->strBegIndices[cursor->stringPos] + inc;
    if(cursor->stringPos > (*firstString) + metrics->clientY/metrics->charY)
      *firstString = (*firstString) + 1;
    break;

  }
  metricsUpdate(metrics, actual, hwnd, *firstString);
  cursorLimit(cursor, buffer, actual, firstString, metrics);
  setCursorPos(cursor, metrics, buffer, actual, firstString);
  InvalidateRect(hwnd, NULL, TRUE);
}