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
} TASKBAR_LOCATION_EDGE;

typedef struct tagTASKBARLOCATION
{
	RECT rcTaskbar;
	RECT rcWorkArea;
	TASKBAR_LOCATION_EDGE uEdge;
} TASKBARLOCATION, * LPTASKBARLOCATION;

typedef enum
{
	WV_WINDOWS_OLDER,
	WV_WINDOWS_2000,
	WV_WINDOWS_XP,
	WV_WINDOWS_VISTA,
	WV_WINDOWS_7,
	WV_WINDOWS_NEWER
} WINDOWS_VERSION;

#if _MSC_VER <= 1200
#define _VECTOR_DATA(v) ((v).begin())
#elif _MSC_VER < 1600
#define _VECTOR_DATA(v) ((v).begin()._Myptr)
#else
#define _VECTOR_DATA(v) ((v).data())
#endif

const wstring ConvertToWideChar(const string szString, int nCodePage = -1);
const wstring ConvertToWideChar(const TByteVector * szData, int nCodePage = -1);
const wstring ConvertToWideChar(LPCSTR szString, size_t nLength, int nCodePage = -1);
const string ConvertToMultiByte(const wstring szString, int nCodePage = -1);
const string ConvertToMultiByte(LPCWSTR szString, size_t nLength, int nCodePage = -1);

BOOL RemoveDirectory(wstring szPath, BOOL fRecurse = FALSE);

wstring Trim(wstring szValue);
BOOL ParseStringMap(const wstring & szInput, TStringStringMap & vMap);
wstring GetModuleFileNameEx();
wstring GetDirname(wstring szPath);
wstring GetBasename(wstring szPath);
wstring GetLongPathName(wstring szPath);
wstring ExpandEnvironmentStrings(wstring szPath);

wstring UrlEncode(wstring szSource);
wstring Format(wstring szFormat, ...);

HFONT GetMessageBoxFont(BOOL fReload = FALSE);
HFONT CreateFontIndirectEx(HFONT hFont, LONG lWeight, BOOL fItalic, BOOL fUnderline);
BOOL FontExists(wstring szFontName);

WINDOWS_VERSION GetWindowsVersion();
LPCWSTR GetLanguageCode();

BOOL RunningWine();
wstring GetWineVersion();
void GetTaskbarLocation(LPTASKBARLOCATION lpLocation);
BOOL GetTaskbarPopupLocation(SIZE sSize, SIZE sOffset, LPRECT lpLocation);
void SnapTaskbarPopupLocation(LPRECT lpLocation, POINT pt);
BOOL RunningRemoteDesktop();
BOOL DegradeVisualPerformance();
void SubclassStaticForLink(HWND hWnd, wstring szUrl = L"");

wstring Base64Encode(TByteVector vData);
TByteVector Base64Decode(wstring szData);

BOOL EncryptString(wstring szValue, wstring & szEncrypted);
BOOL DecryptString(wstring szValue, wstring & szDecrypted);

INT Rand(INT nMin, INT nMax);

bool isword(char _Ch);
bool iswword(wchar_t _Ch);

bool iswalnum(wstring _String);
bool iswalpha(wstring _String);
bool iswcntrl(wstring _String);
bool iswdigit(wstring _String);
bool iswgraph(wstring _String);
bool iswlower(wstring _String);
bool iswprint(wstring _String);
bool iswpunct(wstring _String);
bool iswspace(wstring _String);
bool iswupper(wstring _String);
bool iswword(wstring _String);
bool iswxdigit(wstring _String);
wstring towlower(wstring _String);
wstring towuppoer(wstring _String);

// Manually set debug breaks in code (better than DebugBreak because this
// actually breaks at the point DEBUGBREAK is put and not somewhere
// in a DLL, where there simply is an { int 3 } anyway).

#ifdef _DEBUG

#define DEBUGBREAK __asm { int 3 }

#else // !_DEBUG

#define DEBUGBREAK

#endif

#ifdef _DEBUG

#define new				new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(_Size)			_malloc_dbg((_Size), _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(_Memory, _Size)		_realloc_dbg((void *)(_Memory), (_Size), _NORMAL_BLOCK, __FILE__, __LINE__)
#define free(_Memory)			_free_dbg((void *)(_Memory), _NORMAL_BLOCK)

#endif // _DEBUG

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

#endif // _INC_SUPPORT
