#include "cursor.h"

void cursorInit(cursor* cursor, metrics* metrics, HWND hwnd, storageModel* buffer, viewModel* actual, int* firstString) {
  cursor->storagePos = 0;
  cursor->stringPos = 0;
  CreateCaret(hwnd, NULL, 2, metrics->charY);
  setCursorPos(cursor, metrics, buffer, actual, firstString);
}

void cursorLimit(cursor* cursor, storageModel* buffer, viewModel* actual, int* firstString) {
  cursor->stringPos = min(max(0, cursor->stringPos), actual->strCount - 1);
  int len = (cursor->stringPos == actual->strCount - 1) ? buffer->size - actual->strBegIndices[cursor->stringPos] - 1 : actual->strBegIndices[cursor->stringPos + 1] - actual->strBegIndices[cursor->stringPos];
  if (buffer->buffer[max(0, actual->strBegIndices[cursor->stringPos + 1] - 1)] == '\n')
    len--;
  if (buffer->buffer[max(0, actual->strBegIndices[cursor->stringPos + 1] - 1)] == '\0')
    len--;
  if (buffer->buffer[max(0, actual->strBegIndices[cursor->stringPos + 1] - 2)] == '\r')
    len -= 2;
  cursor->storagePos = min(max(0, cursor->storagePos), actual->strBegIndices[cursor->stringPos] + len);
}

void setCursorPos(cursor* cursor, metrics* metrics, storageModel* buffer, viewModel* actual, int* firstString) {
  cursorLimit(cursor, buffer, actual, firstString);
  SetCaretPos(metrics->charX * (max(0, cursor->storagePos - actual->strBegIndices[cursor->stringPos]) + 1), metrics->charY * (cursor->stringPos - *firstString));
}

void cursorReset(cursor* cursor, metrics* metrics, storageModel* buffer, viewModel* actual, int* firstString) {
  cursor->storagePos = 0;
  cursor->stringPos = 0;
  setCursorPos(cursor, metrics, buffer, actual, firstString);
}