#ifndef __FILE_H__
#define __FILE_H__
#include "architecture.h"
#include "functions.h"
#include "cursor.h"
#include <stdio.h>

int viewModelUpdate(storageModel* buffer, viewModel* actual, int charCount, char* fstr, cursor* cursor);
void loadFile(storageModel* buffer, viewModel* actual, metrics* metrics, HINSTANCE dll, char* path, HWND hwnd, cursor* cursor);
void ofnInit(OPENFILENAMEA* ofn, HWND hwnd, char* path);
void saveFile(storageModel* buffer, char* path);
#endif
