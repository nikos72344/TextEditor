#include "functions.h"

void metricsInit(metrics* metrics, HWND hwnd){
  TEXTMETRIC tm;
  RECT rect;
  HDC hdc = GetDC(hwnd);
  SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
  SetBkMode(hdc, TRANSPARENT);
  GetTextMetrics(hdc, &tm);
  ReleaseDC(hwnd, hdc);

  metrics->charX = tm.tmMaxCharWidth;
  metrics->charY = tm.tmHeight + tm.tmExternalLeading;

  GetClientRect(hwnd, &rect);
  metrics->clientX = rect.right;
  metrics->clientY = rect.bottom;

  metrics->VscrollPos = 0;
  metrics->VscrollMax = 0;
  metrics->VscrollCoef = 1;
  metrics->HscrollPos = 0;
  metrics->HscrollMax = 0;
}

void storageInit(storageModel* buffer) {
  buffer->buffer = (char*)malloc(sizeof(char));
  buffer->buffer[0] = '\0';
  buffer->size = 1;
}

void metricsReset(metrics* metrics, viewModel* actual, HWND hwnd) {
  metrics->VscrollCoef = actual->strCount / 65536 + 1;
  metrics->VscrollMax = max(0, (actual->strCount - 1) / metrics->VscrollCoef);
  if (actual->strCount < metrics->clientY / metrics->charY)
    metrics->VscrollMax = 0;

  metrics->VscrollPos = 0;

  SetScrollRange(hwnd, SB_VERT, 0, metrics->VscrollMax, FALSE);
  SetScrollPos(hwnd, SB_VERT, metrics->VscrollPos, TRUE);

  metrics->HscrollMax = max(0, (actual->maxLen - metrics->clientX / metrics->charX));
  if (actual->maxLen < metrics->clientX / metrics->charX)
    metrics->HscrollMax = 0;

  metrics->HscrollPos = 0;

  SetScrollRange(hwnd, SB_HORZ, 0, metrics->HscrollMax, FALSE);
  SetScrollPos(hwnd, SB_HORZ, metrics->HscrollPos, TRUE);

}

void metricsUpdate(metrics* metrics, viewModel* actual, HWND hwnd, int firstString) {
  metrics->VscrollCoef = actual->strCount / 65536 + 1;
  metrics->VscrollMax = max(0, (actual->strCount - 1) / metrics->VscrollCoef);
  if (actual->strCount < metrics->clientY / metrics->charY)
    metrics->VscrollMax = 0;

  metrics->VscrollPos = min(metrics->VscrollMax, firstString / metrics->VscrollCoef);

  SetScrollRange(hwnd, SB_VERT, 0, metrics->VscrollMax, FALSE);
  SetScrollPos(hwnd, SB_VERT, metrics->VscrollPos, TRUE);

  metrics->HscrollMax = max(0, (actual->maxLen - metrics->clientX / metrics->charX));
  if (actual->maxLen < metrics->clientX / metrics->charX)
    metrics->HscrollMax = 0;

  metrics->HscrollPos = min(metrics->HscrollPos, metrics->HscrollMax);

  SetScrollRange(hwnd, SB_HORZ, 0, metrics->HscrollMax, FALSE);
  SetScrollPos(hwnd, SB_HORZ, metrics->HscrollPos, TRUE);
}

void viewModelInit(viewModel* normal, viewModel* layout) {
  normal->strBegIndices = (int*)malloc(sizeof(int));
  normal->strBegIndices[0] = 0;
  normal->strCount = 0;
  normal->maxLen = 0;
  normal->mode = NORMAL;

  layout->strBegIndices = (int*)malloc(sizeof(int));
  layout->strBegIndices[0] = 0;
  layout->strCount = 0;
  layout->maxLen = 0;
  layout->mode = LAYOUT;
}