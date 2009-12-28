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

#ifndef INC_UTF8CONVERTER
#define INC_UTF8CONVERTER

#pragma once

typedef enum
{
	UTF8S_NEXT_CHAR,
	UTF8S_EAT_BYTES
} UTF8_STATE;

class CUTF8Converter
{
private:
	typedef BYTE utf8_t;
	typedef wchar_t utf16_t;
	typedef DWORD utf32_t;

	typedef vector<utf16_t> TUTF16Vector;
	typedef TUTF16Vector::iterator TUTF16VectorIter;
	typedef TUTF16Vector::const_iterator TUTF16VectorConstIter;

private:
	UTF8_STATE m_nState;
	utf32_t m_cNextChar;
	INT m_nExtraBytes;
	INT m_nExtraBytesLeft;
	static const char m_vTrailingBytes[256];
	static utf32_t m_vOffsetFromUTF8[6];

public:
	CUTF8Converter();
	wstring Parse(TByteVector & vData);
	wstring Parse(LPBYTE lpBytes, size_t cbBytes);

private:
	void Reset();
	void DoNextChar(utf8_t cSource, TUTF16Vector & vResult);
	void DoEatBytes(utf8_t cSource, TUTF16Vector & vResult);
	void DoProcessChar(utf8_t cSource, TUTF16Vector & vResult);
};

#endif // INC_UTF8CONVERTER
