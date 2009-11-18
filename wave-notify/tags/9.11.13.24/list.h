/*
 * This file is part of Google Wave Notifier.
 *
 * Google Wave Notifier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Google Wave Notifier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Google Wave Notifier.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _INC_LIST
#define _INC_LIST

#pragma once

typedef struct tagLIST_ITEM {
	DWORD dwIndex;
	void * lpValue;
	struct tagLIST_ITEM * lpNext;
} LIST_ITEM, * LPLIST_ITEM;

typedef struct tagLIST {
	DWORD dwCount;
	LPLIST_ITEM lpHead;
	LPLIST_ITEM * lppTail;
} LIST, * LPLIST;

typedef struct tagITER {
	LPLIST lpList;
	LPLIST_ITEM lpCurrent;
} ITER, * LPITER;

#ifdef _DEBUG

LPLIST List_DbgCreate(LPCSTR szFile, INT nLine);

#define List_Create() List_DbgCreate(__FILE__, __LINE__)

#else

LPLIST List_Create();

#endif

void List_Destroy(LPLIST lpList);
void List_AddItem(LPLIST lpList, void * lpValue);
LPITER List_Start(LPLIST lpList);
void * List_Next(LPITER lpIter);
void List_End(LPITER lpIter);
void List_CascadeDestroy(LPLIST lpList);
void ** List_ToArray(LPLIST lpList);
void * List_Dequeue(LPLIST lpList);

#define List_Add(lpList, lpValue) List_AddItem((lpList), (void *)(lpValue))
#define List_GetCount(lpList) ((lpList)->dwCount)

#endif // _INC_LIST
