#ifndef __MENU_H_
#define __MENU_H_
#include <Windows.h>

#define IDM_NORMAL 1
#define IDM_LAYOUT 2

#define IDM_OPEN 3
#define IDM_SAVE 4
#define IDM_SAVE_AS 5
#define IDM_EXIT 6

void initMenu(HMENU* menu, HMENU* contextMenu, HWND hwnd);
void freeMenu(HMENU* menu, HMENU* contextMenu);

#endif
