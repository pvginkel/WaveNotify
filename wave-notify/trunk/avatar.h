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

#ifndef _INC_AVATAR
#define _INC_AVATAR

#pragma once

class CAvatar;

typedef map<wstring, CAvatar *> TAvatarMap;
typedef TAvatarMap::iterator TAvatarMapIter;
typedef TAvatarMap::const_iterator TAvatarMapConstIter;

class CAvatar
{
private:
	BITMAPINFO m_vBitmap;
	HBITMAP m_hBitmap;
	SIZE m_szSize;

private:
	CAvatar(SIZE szSize);

public:
	virtual ~CAvatar();

	void Paint(CDC * lpDC, POINT ptLocation);

	static CAvatar * Load(LPCWSTR szResource, LPCWSTR szResourceType, HMODULE hModule, SIZE szSize, wstring szContentType);
	static CAvatar * Create(const TByteVector & vData, SIZE szSize, wstring szContentType);
	static CAvatar * Create(const LPVOID lpData, DWORD cbData, SIZE szSize, wstring szContentType);

private:
	BOOL LoadImage(const LPVOID lpData, DWORD cbData, wstring szContentType);

	static gdImagePtr CreateImage(LPINT lpBits, SIZE szSize);
};

#endif // _INC_AVATAR
