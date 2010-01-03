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

/*
 * Code taken from http://trac.transmissionbt.com/browser/trunk/libtransmission/ConvertUTF.c
 *
 * From the code above, isLegalUTF8 not incorporated.
 */

#define UNI_REPLACEMENT_CHAR 	((utf32_t)0x0000FFFDUL)
#define UNI_MAX_BMP 		((utf32_t)0x0000FFFFUL)
#define UNI_MAX_UTF16 		((utf32_t)0x0010FFFFUL)
#define UNI_MAX_UTF32		((utf32_t)0x7FFFFFFFUL)
#define UNI_MAX_LEGAL_UTF32	((utf32_t)0x0010FFFFUL)

#define UNI_HALF_BASE		((utf32_t)0x0010000UL)
#define UNI_HALF_MASK		((utf32_t)0x3FFUL)
#define UNI_HALF_SHIFT		((utf32_t)10UL)

#define UNI_SUR_HIGH_START  	((utf32_t)0xD800UL)
#define UNI_SUR_HIGH_END    	((utf32_t)0xDBFFUL)
#define UNI_SUR_LOW_START   	((utf32_t)0xDC00UL)
#define UNI_SUR_LOW_END     	((utf32_t)0xDFFFUL)

const char CUTF8Converter::m_vTrailingBytes[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

CUTF8Converter::utf32_t CUTF8Converter::m_vOffsetFromUTF8[6] = {
	0x00000000UL, 0x00003080UL, 0x000E2080UL, 0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

CUTF8Converter::CUTF8Converter()
{
	Reset();
}

void CUTF8Converter::Reset()
{
	m_cNextChar = 0;
	m_nState = UTF8S_NEXT_CHAR;
	m_nExtraBytes = 0;
	m_nExtraBytesLeft = 0;
}

wstring CUTF8Converter::Parse(TByteVector & vData)
{
	return Parse(_VECTOR_DATA(vData), vData.size());
}

wstring CUTF8Converter::Parse(LPBYTE lpBytes, size_t cbBytes)
{
	TUTF16Vector vResult;

	for (size_t cbOffset = 0; cbOffset < cbBytes; cbOffset++)
	{
		utf8_t cSource = lpBytes[cbOffset];

		switch (m_nState)
		{
		case UTF8S_NEXT_CHAR:
			DoNextChar(cSource, vResult);
			break;

		case UTF8S_EAT_BYTES:
			DoEatBytes(cSource, vResult);
			break;
		}
	}

	return wstring(_VECTOR_DATA(vResult), vResult.size());
}

void CUTF8Converter::DoNextChar(utf8_t cSource, TUTF16Vector & vResult)
{
	m_nExtraBytesLeft = m_nExtraBytes = m_vTrailingBytes[cSource];
	m_nState = UTF8S_EAT_BYTES;

	DoEatBytes(cSource, vResult);
}

void CUTF8Converter::DoEatBytes(utf8_t cSource, TUTF16Vector & vResult)
{
	m_cNextChar += cSource;

	if (m_nExtraBytesLeft > 0)
	{
		m_cNextChar <<= 6;
	}

	m_nExtraBytesLeft--;

	if (m_nExtraBytesLeft < 0)
	{
		DoProcessChar(cSource, vResult);
	}
}

void CUTF8Converter::DoProcessChar(utf8_t cSource, TUTF16Vector & vResult)
{
	m_cNextChar -= m_vOffsetFromUTF8[m_nExtraBytes];

	if (m_cNextChar <= UNI_MAX_BMP)
	{
		if (m_cNextChar >= UNI_SUR_HIGH_START && m_cNextChar <= UNI_SUR_LOW_END)
		{
			vResult.push_back(UNI_REPLACEMENT_CHAR);
		}
		else
		{
			vResult.push_back((utf16_t)m_cNextChar);
		}
	}
	else if (m_cNextChar > UNI_MAX_UTF16)
	{
		vResult.push_back(UNI_REPLACEMENT_CHAR);
	}
	else
	{
		m_cNextChar -= UNI_HALF_BASE;
		vResult.push_back((utf16_t)((m_cNextChar >> UNI_HALF_SHIFT) + UNI_SUR_HIGH_START));
		vResult.push_back((utf16_t)((m_cNextChar & UNI_HALF_MASK) + UNI_SUR_LOW_START));
	}

	Reset();
}
