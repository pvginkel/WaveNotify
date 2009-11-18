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

#ifndef _INC_LEGACY
#define _INC_LEGACY

#pragma once

#if _MSC_VER <= 1200

#define wcscpy_s compat_wcscpy_s
#define wcsncpy_s compat_wcsncpy_s
#define memcpy_s compat_memcpy_s
#define _wtof compat__wtof
#define wcstombs_s compat_wcstombs_s
#define mbstowcs_s compat_mbstowcs_s

#define rsize_t size_t

#endif

errno_t compat_wcscpy_s(wchar_t * _Dst, rsize_t _SizeInWords, const wchar_t * _Src);
errno_t compat_wcsncpy_s(wchar_t * _Dst, rsize_t _SizeInWords, const wchar_t * _Src, rsize_t _MaxCount);
errno_t  compat_memcpy_s(void * _Dst, rsize_t _DstSize, const void * _Src, rsize_t _MaxCount);
double compat__wtof(const wchar_t *_Str);
errno_t compat_wcstombs_s(size_t * _PtNumOfCharConverted, char * _Dst, size_t _DstSizeInBytes, const wchar_t * _Src, size_t _MaxCountInBytes);
errno_t compat_mbstowcs_s(size_t * _PtNumOfCharConverted, wchar_t * _DstBuf, size_t _SizeInWords, const char * _SrcBuf, size_t _MaxCount );

#endif // _INC_LEGACY
