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

CAvatar::CAvatar(SIZE szSize)
{
	m_szSize = szSize;
	m_hBitmap = NULL;
}

CAvatar::~CAvatar()
{
	DeleteObject(m_hBitmap);
}

CAvatar * CAvatar::Create(const TByteVector & vData, SIZE szSize, wstring szContentType)
{
	BOOL fSuccess = FALSE;
	CAvatar * lpResult = new CAvatar(szSize);

	if (lpResult->LoadImage(vData, szContentType))
	{
		return lpResult;
	}
	else
	{
		delete lpResult;

		return NULL;
	}

}

BOOL CAvatar::LoadImage(const TByteVector & vData, wstring szContentType)
{
	gdImagePtr lpTarget = NULL;
	gdImagePtr lpSource = NULL;
	HDC hDC = NULL;
	BOOL fSuccess = FALSE;

	m_hBitmap = NULL;

	//
	// Create the memory bitmap.
	//

	memset(&m_vBitmap, 0, sizeof(BITMAPINFO));

	m_vBitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_vBitmap.bmiHeader.biWidth = m_szSize.cx;
	m_vBitmap.bmiHeader.biHeight = m_szSize.cy;
	m_vBitmap.bmiHeader.biPlanes = 1;
	m_vBitmap.bmiHeader.biBitCount = 32;
	m_vBitmap.bmiHeader.biCompression = BI_RGB;
	m_vBitmap.bmiHeader.biSizeImage = 0;
	m_vBitmap.bmiHeader.biXPelsPerMeter = 0;
	m_vBitmap.bmiHeader.biYPelsPerMeter = 0;
	m_vBitmap.bmiHeader.biClrUsed = 0;
	m_vBitmap.bmiHeader.biClrImportant = 0;

	hDC = CreateCompatibleDC(GetDC(NULL));

	if (hDC == NULL)
	{
		goto __end;
	}

	LPBYTE lpBits = NULL;

	m_hBitmap = CreateDIBSection(hDC, &m_vBitmap, DIB_RGB_COLORS, (void **)&lpBits, NULL, 0);

	if (m_hBitmap == NULL)
	{
		goto __end;
	}

	//
	// Copy the avatar to the bitmap.
	//

	lpTarget = CreateImage((LPINT)lpBits, m_szSize);

	if (lpTarget == NULL)
	{
		goto __end;
	}

	LPVOID lpData = _VECTOR_DATA(vData);

	if (szContentType == L"image/png")
	{
		lpSource = gdImageCreateFromPngPtr(vData.size(), lpData);
	}
	else if (szContentType == L"image/jpeg")
	{
		lpSource = gdImageCreateFromJpegPtr(vData.size(), lpData);
	}
	else if (szContentType == L"image/gif")
	{
		lpSource = gdImageCreateFromGifPtr(vData.size(), lpData);
	}
	else
	{
		goto __end;
	}

	if (lpSource == NULL)
	{
		goto __end;
	}

	gdImageCopyResampled(
		lpTarget, lpSource,
		0, 0, 0, 0,
		m_szSize.cx, m_szSize.cy,
		gdImageSX(lpSource), gdImageSY(lpSource)
	);

	fSuccess = TRUE;

__end:
	gdImageDestroy(lpSource);

	//
	// The lpTarget image is manually created, so destroy ourself.
	//

	free(lpTarget->tpixels);
	free(lpTarget);

	DeleteDC(hDC);

	return fSuccess;
}

void CAvatar::Paint(CDC * lpDC, POINT ptLocation)
{
	CDC vSource(CreateCompatibleDC(lpDC->GetHandle()));

	HGDIOBJ hOriginal = vSource.SelectObject(m_hBitmap);

	POINT ptSource = { 0, 0 };

	lpDC->BitBlt(ptLocation, m_szSize, &vSource, ptSource, SRCCOPY);

	vSource.SelectObject(hOriginal);

	DeleteDC(vSource.GetHandle());
}

gdImagePtr CAvatar::CreateImage(LPINT lpBits, SIZE szSize)
{
	gdImagePtr lpImage = (gdImagePtr)malloc(sizeof(gdImage));

	memset(lpImage, 0, sizeof(gdImage));

	lpImage->tpixels = (int **)malloc(sizeof(int *) * szSize.cy);

	lpImage->polyInts = 0;
	lpImage->polyAllocated = 0;
	lpImage->brush = 0;
	lpImage->tile = 0;
	lpImage->style = 0;

	LPINT lpRowPtr = lpBits + ((szSize.cy - 1) * (szSize.cx));

	for (INT i = 0; i < szSize.cy; i++)
	{
		lpImage->tpixels[i] = lpRowPtr;

		lpRowPtr -= szSize.cx;
	}

	lpImage->sx = szSize.cx;
	lpImage->sy = szSize.cy;
	lpImage->transparent = (-1);
	lpImage->interlace = 0;
	lpImage->trueColor = 1;
	lpImage->saveAlphaFlag = 0;
	lpImage->alphaBlendingFlag = 1;
	lpImage->thick = 1;
	lpImage->AA = 0;
	lpImage->cx1 = 0;
	lpImage->cy1 = 0;
	lpImage->cx2 = lpImage->sx - 1;
	lpImage->cy2 = lpImage->sy - 1;

	return lpImage;
}
