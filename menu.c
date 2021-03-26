#include "menu.h"

void initMenu(HMENU* menu, HMENU* contextMenu, HWND hwnd) {
  HMENU temp;
  *menu = CreateMenu();
  *contextMenu = CreatePopupMenu();
  temp = CreateMenu();
  AppendMenuA(temp, MF_STRING, IDM_OPEN, "Open...");
  AppendMenuA(temp, MF_STRING, IDM_SAVE_AS, "Save as...");
  AppendMenuA(temp, MF_STRING, IDM_SAVE, "Save");
  AppendMenuA(temp, MF_SEPARATOR, 0, NULL);
  AppendMenuA(temp, MF_STRING, IDM_EXIT, "Exit");
  AppendMenuA(*menu, MF_POPUP, (UINT)temp, "File");
  AppendMenuA(*contextMenu, MF_POPUP, (UINT)temp, "File");
  temp = CreateMenu();
  AppendMenuA(temp, MF_STRING | MF_CHECKED, IDM_NORMAL, "Normal");
  AppendMenuA(temp, MF_STRING | MF_UNCHECKED, IDM_LAYOUT, "Layout");
  AppendMenuA(*menu, MF_POPUP, (UINT)temp, "View");
  SetMenu(hwnd, *menu);
  AppendMenuA(*contextMenu, MF_POPUP, (UINT)temp, "View");
}

void freeMenu(HMENU* menu, HMENU* contextMenu) {
  DestroyMenu(*menu);
  DestroyMenu(*contextMenu);
}