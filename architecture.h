#ifndef __ARHITECTURE_H__
#define __ARHITECTURE_H__
#include <Windows.h>

#pragma warning (disable: 4996)

typedef struct {
  char* buffer;
  int size;
} storageModel;

typedef enum {
  NORMAL,
  LAYOUT
} mode;

typedef struct {
  int* strBegIndices;
  int strCount;
  int maxLen;
  mode mode;
} viewModel;

typedef struct {
  int charX;
  int charY;
  int clientX;
  int clientY;
  int VscrollPos;
  int VscrollMax;
  int VscrollCoef;
  int HscrollPos;
  int HscrollMax;
} metrics;

typedef char* (*ReadText_t) (const char*, int*);

#endif