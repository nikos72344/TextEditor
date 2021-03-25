#ifndef __EDITOR_H__
#define __EDITOR_H__
#include "architecture.h"
#include "cursor.h"

BOOLEAN addChar(storageModel* buffer, cursor* cursor, char ch);
void addReturn(storageModel* buffer, cursor* cursor);
BOOLEAN deleteChar(storageModel* buffer, cursor* cursor);

#endif
