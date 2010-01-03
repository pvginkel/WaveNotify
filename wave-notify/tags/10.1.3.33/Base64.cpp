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

static const WCHAR g_vEncodeTable[64] =
{
	L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H',
	L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P',
	L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X',
	L'Y', L'Z', L'a', L'b', L'c', L'd', L'e', L'f',
	L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n',
	L'o', L'p', L'q', L'r', L's', L't', L'u', L'v',
	L'w', L'x', L'y', L'z', L'0', L'1', L'2', L'3',
	L'4', L'5', L'6', L'7', L'8', L'9', L'+', L'/'
};

static LPCWSTR g_vEncodeTableEnd = g_vEncodeTable + _ARRAYSIZE(g_vEncodeTable);

static const BYTE g_vDecodeTable[128] =
{
	-1, -1, -1, -1, -1, -1, -1, -1, // 0
	-1, -1, -1, -1, -1, -1, -1, -1, // 8
	-1, -1, -1, -1, -1, -1, -1, -1, // 16
	-1, -1, -1, -1, -1, -1, -1, -1, // 24
	-1, -1, -1, -1, -1, -1, -1, -1, // 32
	-1, -1, -1, 62, -1, -1, -1, 63, // 40
	52, 53, 54, 55, 56, 57, 58, 59, // 48
	60, 61, -1, -1, -1,  0, -1, -1, // 56
	-1,  0,  1,  2,  3,  4,  5,  6, // 64
	7,  8,  9, 10, 11, 12, 13, 14, // 72
	15, 16, 17, 18, 19, 20, 21, 22, // 80
	23, 24, 25, -1, -1, -1, -1, -1, // 88
	-1, 26, 27, 28, 29, 30, 31, 32, // 96
	33, 34, 35, 36, 37, 38, 39, 40, // 104
	41, 42, 43, 44, 45, 46, 47, 48, // 112
	49, 50, 51, -1, -1, -1, -1, -1  // 120
};

wstring Base64Encode(TByteVector vData)
{
	TByteVectorIter iter = vData.begin();
	INT nLineSize = 0;
	wstringstream szResult;

	INT nBytes;
	do
	{
		DWORD dwInput = 0;

		// Get the next three nBytes into "in" (and count how many we actually get).

		nBytes = 0;

		for(; nBytes < 3 && iter != vData.end(); ++nBytes, ++iter)
		{
			dwInput <<= 8;
			dwInput += *iter;
		}

		// Convert to base64

		INT nBits = nBytes*8;

		while (nBits > 0)
		{
			nBits -= 6;

			const BYTE nIndex = ((nBits < 0) ? dwInput << -nBits : dwInput >> nBits) & 0x3F;

			szResult << g_vEncodeTable[nIndex];

			++nLineSize;
		}

		// Ensure proper line length

		if (nLineSize >= 76)
		{
			szResult << L"\r\n";

			nLineSize = 0;
		}
	}
	while (nBytes == 3);


	// Add pad characters if necessary

	if (nBytes > 0)
	{
		for(int i=nBytes; i < 3; ++i)
		{
			szResult << '=';
		}
	}

	return szResult.str();
}

TByteVector Base64Decode(wstring szData)
{
	TByteVector vResult;
	LPCWSTR szChars = szData.c_str();

	INT nChars;

	do
	{
		BYTE vInput[4] = { 0, 0, 0, 0 };

		// Get four characters

		nChars = 0;

		while (nChars < 4 && *szChars)
		{
			WCHAR nChar = (BYTE)*szChars;

			// Pad character marks the end of the stream

			if (nChar == L'=')
			{
				break; 
			}

			szChars++;

			if (find(g_vEncodeTable, g_vEncodeTableEnd, nChar) != g_vEncodeTableEnd)
			{
				vInput[nChars] = g_vDecodeTable[nChar];

				nChars++;
			}
		}

		// Output the binary data

		if (nChars >= 2)
		{
			vResult.push_back((vInput[0] << 2) + (vInput[1] >> 4));

			if (nChars >= 3)
			{
				vResult.push_back((vInput[1] << 4) + (vInput[2] >> 2));

				if (nChars >= 4)
				{
					vResult.push_back((vInput[2] << 6) + vInput[3]);
				}
			}
		}
	}
	while (nChars == 4);

	return vResult;
}
