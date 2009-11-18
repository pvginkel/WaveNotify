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

#ifndef _INC_SUPPORT
#define _INC_SUPPORT

#pragma once

typedef enum
{
	TLE_LEFT,
	TLE_TOP,
	TLE_RIGHT,
	TLE_BOTTOM
} TASKBARLOCATIONEDGE;

typedef struct tagTASKBARLOCATION
{
	RECT rTaskbar;
	RECT rWorkArea;
	TASKBARLOCATIONEDGE uEdge;
} TASKBARLOCATION, * LPTASKBARLOCATION;

#ifdef _DEBUG

#define _CRTDBG_MAP_malloc
// The new creates problems, so we ignore it
#define _MFC_OVERRIDES_NEW

#include <crtdbg.h>

#else

#include <malloc.h>

#endif // _DEBUG

// Manually set debug breaks in code (better than DebugBreak because this
// actually breaks at the point DEBUGBREAK is put and not somewhere
// in a DLL, where there simply is an { int 3 } anyway).

#ifdef _DEBUG

#define DEBUGBREAK __asm { int 3 }

#else // !_DEBUG

#define DEBUGBREAK

#endif

// These are missing in the VS6 Platform SDK

#ifndef _ARRAYSIZE
#define _ARRAYSIZE(a) (sizeof((a)) / sizeof((a)[0]))
#endif

#ifdef _DEBUG

#ifndef _ASSERT_EXPR

#define _ASSERT_EXPR(expr, msg) \
        do { if (!(expr) && \
                (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, msg))) \
             _CrtDbgBreak(); } while (0)

#endif // _ASSERT_EXPR

#ifndef _CRT_WIDE

#define __CRT_WIDE(_String) L ## _String
#define _CRT_WIDE(_String) __CRT_WIDE(_String)

#endif // _CRT_WIDE

#endif // _DEBUG

#ifdef _DEBUG

// Use this for verbose debugging

#define _VALIDATE_RETURN_VOID(expr, errorcode)					\
{										\
        int _result = !!(expr);							\
										\
        _ASSERT_EXPR(_result, _CRT_WIDE(#expr));				\
										\
	if (!_result) {								\
		errno = errorcode;						\
		return;								\
        }									\
}										\

#define _VALIDATE_RETURN(expr, errorcode, retexpr)				\
{										\
        int _result = !!(expr);							\
										\
        _ASSERT_EXPR(_result, _CRT_WIDE(#expr));				\
										\
	if (!_result) {								\
		errno = errorcode;						\
		return (retexpr);						\
        }									\
}										\

#else // NDEBUG

#define _VALIDATE_RETURN_VOID(expr, errorcode)
#define _VALIDATE_RETURN(expr, errorcode, retexpr)

#endif

#ifndef _ERRCODE_DEFINED
#define _ERRCODE_DEFINED
typedef int errno_t;
#endif

#ifndef IS_INTRESOURCE
#define IS_INTRESOURCE(_r) ((((ULONG_PTR)(_r)) >> 16) == 0)
#endif

// Calculate the distance between two pointers; can return negative values

#ifdef _WIN64

#define DISTANCE(p1, p2) ((long)((__int64)(unsigned __int64)(p2) - (__int64)(unsigned __int64)(p1)))

#else // !_WIN64

#define DISTANCE(p1, p2) ((long)(_W64 unsigned long)(p2) - (long)(_W64 unsigned long)(p1))

#endif // _WIN64

LPSTR dupwcstombs(LPCWSTR szSource);
LPWSTR dupmbstowcs(LPCSTR szSource);

LPWSTR duputf8towcs(LPCSTR szSource);
LPWSTR duputf8towcsn(LPCSTR szSource, size_t uMaxLength);

#define ALLOCA(_Type) \
	( ( _Type * ) ( safe_malloc_zero ( sizeof ( _Type ) ) ) )

#define ALLOC(_Size) \
	( safe_malloc( _Size ) )

#define REALLOC(_Memory, _NewSize) \
	( safe_realloc( (LPVOID)( _Memory ) , ( _NewSize ) ) )

#define FREE(_Memory) \
	( safe_free( (LPVOID)( _Memory ) ) )

#if defined(_DEBUG) && !defined(EXCLUSE_MALLOC_DBG_MAPPINGS)

#define safe_malloc(_Size) \
	dbg_safe_malloc(_Size, _NORMAL_BLOCK, __FILE__, __LINE__)

#define safe_malloc_zero(_Size) \
	dbg_safe_malloc_zero(_Size, _NORMAL_BLOCK, __FILE__, __LINE__)

#define safe_realloc(_Memory, _NewSize) \
	dbg_safe_realloc(_Memory, _NewSize, _NORMAL_BLOCK, __FILE__, __LINE__)

#define safe_free(_Memory) \
	dbg_safe_free(_Memory, _NORMAL_BLOCK)

#endif

#ifdef _DEBUG

void * dbg_safe_malloc(size_t _Size, int _BlockType,
		       const char * _Filename, int _LineNumber);
void * dbg_safe_malloc_zero(size_t _Size, int _BlockType,
		       const char * _Filename, int _LineNumber);
void * dbg_safe_realloc(void * _Memory, size_t _NewSize, int _BlockType,
			const char * _Filename, int _LineNumber);
void dbg_safe_free(void * _Memory, int _BlockType);

#else // !_DEBUG

void * safe_malloc(size_t _Size);
void * safe_malloc_zero(size_t _Size);
void * safe_realloc(void * _Memory, size_t _NewSize);
void safe_free(void * _Memory);

#endif // _DEBUG

HFONT GetMessageBoxFont();
HFONT GetBoldFont(HFONT hFont);
void GetTaskbarLocation(LPTASKBARLOCATION lpLocation);
BOOL GetTaskbarPopupLocation(SIZE sSize, SIZE sOffset, LPRECT lpLocation);
BOOL RunningWine();
BOOL RunningRemoteDesktop();
BOOL DegradeVisualPerformance();
void OpenUrl(LPWSTR szUrl);
void SnapTaskbarPopupLocation(LPRECT lpLocation, POINT p);

#endif // _INC_SUPPORT
