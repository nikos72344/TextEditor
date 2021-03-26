#include "file.h"

void ofnInit(OPENFILENAMEA* ofn, HWND hwnd, char* path)
{
  static char szFilter[] = "Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0";
  ZeroMemory(ofn, sizeof(OPENFILENAMEA));
  ofn->lStructSize = sizeof(OPENFILENAMEA);
  ofn->hwndOwner = hwnd;
  ofn->hInstance = NULL;
  ofn->lpstrFilter = szFilter;
  ofn->lpstrCustomFilter = NULL;
  ofn->nMaxCustFilter = 0;
  ofn->nFilterIndex = 0;
  ofn->lpstrFile = path;
  ofn->nMaxFile = _MAX_PATH;
  ofn->nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
  ofn->lpstrInitialDir = NULL;
  ofn->lpstrTitle = NULL;
  ofn->Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  ofn->nFileOffset = 0;
  ofn->nFileExtension = 0;
  ofn->lpstrDefExt = "txt";
  ofn->lCustData = 0L;
  ofn->lpfnHook = NULL;
  ofn->lpTemplateName = NULL;
}

int viewModelUpdate(storageModel* buffer, viewModel* actual, int charCount, char* fstr, cursor* cursor) {
  int i, firstString = 0;
  if (actual->strBegIndices != NULL)
    free(actual->strBegIndices);
  actual->strBegIndices = (int*)malloc(buffer->size * sizeof(int));
  if (actual->strBegIndices == NULL)
    return 0;
  actual->strBegIndices[0] = 0;
  actual->strCount = 1;
  actual->maxLen = 0;
  for (i = 0; i < buffer->size; i++) {
    int len = i - actual->strBegIndices[actual->strCount - 1];
    if (buffer->buffer[i] == '\n' || (actual->mode == LAYOUT && len >= (charCount - 2))) {
      actual->strBegIndices[actual->strCount] = i + 1;
      len++;
      if (actual->maxLen < len)
        actual->maxLen = len;
      actual->strCount++;
    }
    if ((buffer->buffer[i] == '\0') && (actual->maxLen < len)) 
      actual->maxLen = ++len;
    if (fstr == &buffer->buffer[i])
      firstString = actual->strCount - 1;
    if (i == cursor->storagePos)
      cursor->stringPos = actual->strCount - 1;
  }
  if (actual->mode == LAYOUT)
    actual->maxLen = 0;
  printf("%i %i\n", cursor->storagePos, cursor->stringPos);
  return firstString;
}

void saveFile(storageModel* buffer, char* path) {
  FILE* saveFile = fopen(path, "wb");
  if (saveFile == NULL)
    return;
  int i = 0;
  for (i = 0; i < buffer->size; i++)
  {
    //if (buffer->buffer[i] == '\r')
      //continue;
    fwrite(&buffer->buffer[i], sizeof(char), 1, saveFile);
  }
  fclose(saveFile);
}

void loadFile(storageModel* buffer, viewModel* actual, metrics* metrics, HINSTANCE dll, char* path, HWND hwnd, cursor* cursor) {
  static ReadText_t read;
  int size = 0, temp = 0;
  char* data = NULL;
  if (dll == NULL)
    return;
  if ((read = (ReadText_t)GetProcAddress(dll, "ReadText")) == NULL)
    return;
  if ((data = read(path, &size)) == NULL)
    return;
  //if (buffer->buffer != NULL)
    //free(buffer->buffer);
  buffer->buffer = data;
  buffer->size = size;
  viewModelUpdate(buffer, actual, metrics->clientX / metrics->charX, buffer->buffer, cursor);
  metricsReset(metrics, actual, hwnd);
  cursorReset(cursor, metrics, buffer, actual, &temp);
}

