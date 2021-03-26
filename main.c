#include "architecture.h"
#include "functions.h"
#include "file.h"
#include "menu.h"
#include "cursor.h"
#include "editor.h"
#include <stdio.h>

/* Declare Windows procedure */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/* Make the class name into a global variable */
char szClassName[] = "CodeBlocksWindowsApp";

int WINAPI WinMain(HINSTANCE hThisInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpszArgument,
  int nCmdShow)
{
  HWND hwnd; /* This is the handle for our window */
  MSG messages; /* Here messages to the application are saved */
  WNDCLASSEXA wincl; /* Data structure for the windowclass */

  /* The Window structure */
  wincl.hInstance = hThisInstance;
  wincl.lpszClassName = szClassName;
  wincl.lpfnWndProc = WindowProcedure; /* This function is called by windows */
  wincl.style = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC; /* Catch double-clicks */
  wincl.cbSize = sizeof(WNDCLASSEX);

  /* Use default icon and mouse-pointer */
  wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
  wincl.lpszMenuName = NULL; /* No menu */
  wincl.cbClsExtra = 0; /* No extra bytes after the window class */
  wincl.cbWndExtra = 0; /* structure or the window instance */
  /* Use Windows's default colour as the background of the window */
  wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

  /* Register the window class, and if it fails quit the program */
  if (!RegisterClassExA(&wincl))
    return 0;

  /* The class is registered, let's create the program*/
  hwnd = CreateWindowExA(
    0, /* Extended possibilites for variation */
    szClassName, /* Classname */
    "Code::Blocks Template Windows App", /* Title Text */
    WS_OVERLAPPEDWINDOW, /* default window */
    CW_USEDEFAULT, /* Windows decides the position */
    CW_USEDEFAULT, /* where the window ends up on the screen */
    544, /* The programs width */
    375, /* and height in pixels */
    HWND_DESKTOP, /* The window is a child-window to desktop */
    NULL, /* No menu */
    hThisInstance, /* Program Instance handler */
    lpszArgument /* No Window Creation data */
  );

  AllocConsole();
  freopen("CONIN$", "r", stdin);
  freopen("CONOUT$", "w", stdout);
  freopen("CONOUT$", "w", stderr);


  /* Make the window visible on the screen */
  ShowWindow(hwnd, nCmdShow);

  /* Run the message loop. It will run until GetMessage() returns 0 */
  while (GetMessage(&messages, NULL, 0, 0))
  {
    /* Translate virtual-key messages into character messages */
    TranslateMessage(&messages);
    /* Send message to WindowProcedure */
    DispatchMessage(&messages);
  }

  /* The program return-value is 0 - The value that PostQuitMessage() gave */
  return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  static storageModel buffer;
  static viewModel* actual, normal, layout;
  static metrics metrics;
  static cursor cursor;
  static char filePath[_MAX_PATH], saveAsPath[_MAX_PATH];
  static HINSTANCE readDll;
  static ReadText_t ReadText;
  static HMENU contextMenu, menu;
  static OPENFILENAMEA openOfn, saveAsOfn;
  static int firstString;
  int i, firstStringInc = 0, HscrollInc = 0;
  HDC hdc;
  PAINTSTRUCT ps;
  CREATESTRUCT* ptr = NULL;
  switch (message) {
  case WM_CREATE:
    strcpy(filePath, "Default.txt");
    strcpy(saveAsPath, "Default.txt");
    viewModelInit(&normal, &layout);
    metricsInit(&metrics, hwnd);
    storageInit(&buffer);
    actual = &normal;
    cursorInit(&cursor, &metrics, hwnd, &buffer, actual, &firstString);
    readDll = LoadLibraryA("ReadData");

    ptr = (CREATESTRUCT*)lParam;
    if (ptr->lpCreateParams != NULL && strcmp("", ptr->lpCreateParams))
      strcpy(filePath, ptr->lpCreateParams);

    ofnInit(&openOfn, hwnd, filePath);
    ofnInit(&saveAsOfn, hwnd, saveAsPath);
    loadFile(&buffer, actual, &metrics, readDll, filePath, hwnd, &cursor);
    initMenu(&menu, &contextMenu, hwnd);
    break;

  case WM_SIZE:
    metrics.clientX = LOWORD(lParam);
    metrics.clientY = HIWORD(lParam);

    if (actual->mode == LAYOUT)
      firstString = viewModelUpdate(&buffer, actual, metrics.clientX / metrics.charX, &buffer.buffer[layout.strBegIndices[firstString]], &cursor);

    setCursorPos(&cursor, &metrics, &buffer, actual, &firstString);
    metricsUpdate(&metrics, actual, hwnd, firstString);
    break;
  case WM_PAINT:
    hdc = BeginPaint(hwnd, &ps);
    for (i = firstString; i < min(firstString + metrics.clientY / metrics.charY + 1, actual->strCount); i++) {
      int len = (i == actual->strCount - 1) ? buffer.size - actual->strBegIndices[i] : actual->strBegIndices[i + 1] - actual->strBegIndices[i];
      TextOutA(hdc, metrics.charX * (1 - metrics.HscrollPos), metrics.charY * (i - firstString), &buffer.buffer[actual->strBegIndices[i]], len);
    }
    EndPaint(hwnd, &ps);
    break;
  case WM_VSCROLL:
    switch (LOWORD(wParam))
    {
    case SB_TOP:
      firstStringInc = -firstString;
      break;

    case SB_BOTTOM:
      firstStringInc = actual->strCount - firstString;
      break;

    case SB_LINEUP: 
      firstStringInc = -1;
      break;

    case SB_LINEDOWN: 
      firstStringInc = 1;
      break;

    case SB_PAGEUP: 
      firstStringInc = min(-1, -metrics.clientY / metrics.charY);
      break;

    case SB_PAGEDOWN: 
      firstStringInc = max(1, metrics.clientY / metrics.charY);
      break;

    case SB_THUMBTRACK: 
      firstStringInc = HIWORD(wParam) * metrics.VscrollCoef - firstString;
      break;

    default:
      firstStringInc = 0;
    }
    firstStringInc = max(-firstString, min(firstStringInc, actual->strCount - firstString - 1));

    if (firstStringInc != 0)
    {
      firstString += firstStringInc;
      ScrollWindow(hwnd, 0, -metrics.charY * firstStringInc, NULL, NULL);
      metricsUpdate(&metrics, actual, hwnd, firstString);
      UpdateWindow(hwnd);
    }
    break;

  case WM_HSCROLL:
    switch (LOWORD(wParam))
    {
    case SB_LINEUP:
      HscrollInc = -1;
      break;

    case SB_LINEDOWN:
      HscrollInc = 1;
      break;

    case SB_THUMBTRACK:
      HscrollInc = HIWORD(wParam) - metrics.HscrollPos;
      break;

    default:
      HscrollInc = 0;
    }
    HscrollInc = max(-metrics.HscrollPos, min(HscrollInc, metrics.HscrollMax - metrics.HscrollPos));

    if (HscrollInc != 0)
    {
      metrics.HscrollPos += HscrollInc;
      ScrollWindow(hwnd, -metrics.charX * HscrollInc, 0, NULL, NULL);
      SetScrollPos(hwnd, SB_HORZ, metrics.HscrollPos, TRUE);
      UpdateWindow(hwnd);
    }
    break;

  case WM_KEYDOWN:
    switch (wParam) {
    case VK_PRIOR:
      SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0L);
      break;
    case VK_NEXT:
      SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
      break;
    case VK_RETURN:
      addReturn(&buffer, &cursor);
      viewModelUpdate(&buffer, actual, metrics.clientX / metrics.charX, &buffer.buffer[actual->strBegIndices[firstString]], &cursor);
      setCursorPos(&cursor, &metrics, &buffer, actual, &firstString);
      metricsUpdate(&metrics, actual, hwnd, firstString);
      InvalidateRect(hwnd, NULL, TRUE);
      break;

    case VK_BACK:
      if (deleteChar(&buffer, &cursor))
      {
        viewModelUpdate(&buffer, actual, metrics.clientX / metrics.charX, &buffer.buffer[actual->strBegIndices[firstString]], &cursor);
        setCursorPos(&cursor, &metrics, &buffer, actual, &firstString);
        metricsUpdate(&metrics, actual, hwnd, firstString);
        InvalidateRect(hwnd, NULL, TRUE);
      }
      break;
    }
    cursorMove(&cursor, actual, &buffer, &metrics, wParam, &firstString);
    break;

  case WM_CONTEXTMENU:
    TrackPopupMenu(contextMenu, 0, LOWORD(lParam), HIWORD(lParam), 0, hwnd, NULL);
    break;

  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDM_OPEN:
      if (GetOpenFileNameA(&openOfn)) {
        loadFile(&buffer, actual, &metrics, readDll, filePath, hwnd, &cursor);
        firstString = 0;
        InvalidateRect(hwnd, NULL, TRUE);
      }
      break;
    case IDM_SAVE:
      saveFile(&buffer, filePath);
      break;
    case IDM_SAVE_AS:
      strcpy(saveAsPath, filePath);
      if (GetSaveFileNameA(&saveAsOfn))
        saveFile(&buffer, saveAsPath);
      break;
    case IDM_EXIT:
      SendMessage(hwnd, WM_DESTROY, 0, 0);
      break;
    case IDM_NORMAL:
      if (actual->mode == LAYOUT) {
        CheckMenuItem(menu, IDM_NORMAL, MF_CHECKED);
        CheckMenuItem(menu, IDM_LAYOUT, MF_UNCHECKED);

        actual = &normal;

        firstString = viewModelUpdate(&buffer, actual, metrics.clientX / metrics.charX, &buffer.buffer[layout.strBegIndices[firstString]], &cursor);
        setCursorPos(&cursor, &metrics, &buffer, actual, &firstString);
        metricsUpdate(&metrics, actual, hwnd, firstString);
        InvalidateRect(hwnd, NULL, TRUE);
      }
      break;

    case IDM_LAYOUT:
      if (actual->mode == NORMAL) {
        CheckMenuItem(menu, IDM_LAYOUT, MF_CHECKED);
        CheckMenuItem(menu, IDM_NORMAL, MF_UNCHECKED);

        actual = &layout;

        firstString = viewModelUpdate(&buffer, actual, metrics.clientX / metrics.charX, &buffer.buffer[normal.strBegIndices[firstString]], &cursor);
        setCursorPos(&cursor, &metrics, &buffer, actual, &firstString);
        metricsUpdate(&metrics, actual, hwnd, firstString);
        InvalidateRect(hwnd, NULL, TRUE);
      }
      break;

    }
    break;
  case WM_SETFOCUS:
    CreateCaret(hwnd, NULL, 2, metrics.charY);
    setCursorPos(&cursor, &metrics, &buffer, actual, &firstString);
    ShowCaret(hwnd);
    break;

  case WM_KILLFOCUS:
    HideCaret(hwnd);
    DestroyCaret();
    break;

  case WM_LBUTTONDOWN:
    cursor.stringPos = HIWORD(lParam) / metrics.charY + firstString;
    cursor.storagePos = max(0, LOWORD(lParam) / metrics.charX - 1) + actual->strBegIndices[cursor.stringPos];
    setCursorPos(&cursor, &metrics, &buffer, actual, &firstString);
    break;

  case WM_MOUSEMOVE:
    if (wParam == MK_LBUTTON) {
      cursor.stringPos = HIWORD(lParam) / metrics.charY + firstString;
      cursor.storagePos = max(0, LOWORD(lParam) / metrics.charX - 1) + actual->strBegIndices[cursor.stringPos];
      setCursorPos(&cursor, &metrics, &buffer, actual, &firstString);
    }
    break;

  case WM_CHAR:
    if (addChar(&buffer, &cursor, wParam))
    {
      cursor.storagePos++;
      viewModelUpdate(&buffer, actual, metrics.clientX / metrics.charX, &buffer.buffer[actual->strBegIndices[firstString]], &cursor);
      setCursorPos(&cursor, &metrics, &buffer, actual, &firstString);
      metricsUpdate(&metrics, actual, hwnd, firstString);
      InvalidateRect(hwnd, NULL, TRUE);
    }
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    //if (buffer.buffer != NULL)
      //free(buffer.buffer);
    if (normal.strBegIndices != NULL)
      free(normal.strBegIndices);
    if (layout.strBegIndices != NULL)
      free(layout.strBegIndices);
    FreeLibrary(readDll);
    freeMenu(&menu, &contextMenu);
    DestroyCaret();
    break;
  default:
    return DefWindowProc(hwnd, message, wParam, lParam);
  }
  return 0;
}