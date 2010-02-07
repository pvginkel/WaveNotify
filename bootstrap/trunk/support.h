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

#ifdef _DEBUG

#define new				new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(_Size)			_malloc_dbg((_Size), _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(_Memory, _Size)		_realloc_dbg((void *)(_Memory), (_Size), _NORMAL_BLOCK, __FILE__, __LINE__)
#define free(_Memory)			_free_dbg((void *)(_Memory), _NORMAL_BLOCK)

#endif // _DEBUG

#endif // _INC_SUPPORT
