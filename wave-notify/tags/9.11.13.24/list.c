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

#include "stdafx.h"
#include "include.h"

#ifdef _DEBUG
LPLIST List_DbgCreate(LPCSTR szFile, INT nLine)
#else
LPLIST List_Create()
#endif
{
	LPLIST lpResult;

#ifdef _DEBUG
	lpResult = (LPLIST)dbg_safe_malloc_zero(sizeof(LIST), _NORMAL_BLOCK, szFile, nLine);
#else
	lpResult = ALLOCA(LIST);
#endif

	lpResult->lppTail = &lpResult->lpHead;

	return lpResult;
}

void List_Destroy(LPLIST lpList)
{
	LPLIST_ITEM lpItem;
	LPLIST_ITEM lpDelete;

	lpItem = lpList->lpHead;

	while (lpItem != NULL)
	{
		lpDelete = lpItem;

		lpItem = lpItem->lpNext;

		FREE(lpDelete);
	}

	FREE(lpList);
}

void List_AddItem(LPLIST lpList, void * lpValue)
{
	LPLIST_ITEM lpItem;

	lpItem = ALLOCA(LIST_ITEM);

	lpItem->dwIndex = lpList->dwCount;
	lpItem->lpValue = lpValue;

	*lpList->lppTail = lpItem;

	lpList->lppTail = &lpItem->lpNext;

	lpList->dwCount++;
}

LPITER List_Start(LPLIST lpList)
{
	LPITER lpResult;

	lpResult = ALLOCA(ITER);

	lpResult->lpList = lpList;

	return lpResult;
}

void * List_Next(LPITER lpIter)
{
	if (lpIter->lpCurrent == NULL)
	{
		lpIter->lpCurrent = lpIter->lpList->lpHead;
	}
	else
	{
		lpIter->lpCurrent = lpIter->lpCurrent->lpNext;
	}

	if (lpIter->lpCurrent == NULL)
	{
		return NULL;
	}
	else
	{
		return lpIter->lpCurrent->lpValue;
	}
}

void List_End(LPITER lpIter)
{
	FREE(lpIter);
}

void List_CascadeDestroy(LPLIST lpList)
{
	LPITER lpIter;
	void * lpValue;

	lpIter = List_Start(lpList);

	while (( lpValue = List_Next(lpIter) ) != 0)
	{
		FREE(lpValue);
	}

	List_End(lpIter);

	List_Destroy(lpList);
}

void ** List_ToArray(LPLIST lpList)
{
	void ** lpResult;
	INT i;
	LPLIST_ITEM lpItem;

	lpResult = (void **)ALLOC(lpList->dwCount * sizeof(void *));

	for (i = 0, lpItem = lpList->lpHead; lpItem != NULL; i++, lpItem = lpItem->lpNext)
	{
		lpResult[i] = lpItem->lpValue;
	}

	return lpResult;
}

void * List_Dequeue(LPLIST lpList)
{
	void * lpResult;
	LPLIST_ITEM lpDelete;

	if (lpList->dwCount == 0)
	{
		return NULL;
	}

	lpDelete = lpList->lpHead;
	lpList->lpHead = lpDelete->lpNext;
	lpList->dwCount--;

	lpResult = lpDelete->lpValue;

	FREE(lpDelete);

	if (lpList->dwCount == 0)
	{
		lpList->lppTail = &lpList->lpHead;
	}

	return lpResult;
}
