#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__
#include "architecture.h"

void metricsInit(metrics* metrics, HWND hwnd);
void viewModelInit(viewModel* normal, viewModel* layout);
void metricsReset(metrics* metrics, viewModel* actual, HWND hwnd);
void metricsUpdate(metrics* metrics, viewModel* actual, HWND hwnd, int firstString);
void storageInit(storageModel* buffer);
#endif
