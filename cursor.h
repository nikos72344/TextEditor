#ifndef __CURSOR_H__
#define __CURSOR_H__
#include "architecture.h"

typedef struct {
  int storagePos;
  int stringPos;
} cursor;

void cursorInit(cursor* cursor, metrics* metrics, HWND hwnd, storageModel* buffer, viewModel* actual, int* firstString);
void setCursorPos(cursor* cursor, metrics* metrics, storageModel* buffer, viewModel* actual, int* firstString);
void cursorReset(cursor* cursor, metrics* metrics, storageModel* buffer, viewModel* actual, int* firstString);
void cursorMove(cursor* cursor, viewModel* actual, storageModel* buffer, metrics* metrics, WPARAM wParam, int* firstString, HWND hwnd);
#endif
