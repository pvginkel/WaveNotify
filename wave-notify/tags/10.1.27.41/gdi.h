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

#ifndef _INC_GDI
#define _INC_GDI

#pragma once

class CDC
{
private:
	HDC m_hDC;

public:
	CDC(HDC hDC) {
		ASSERT(hDC != NULL);

		m_hDC = hDC;
	}
	void FrameRect(LPRECT lprc, HBRUSH hBrush) const {
		CHECK_PTR(lprc);
		CHECK_HANDLE(hBrush);

		int nResult = ::FrameRect(m_hDC, lprc, hBrush);

		CHECK_NE_0(nResult);
	}
	void FillRect(LPRECT lprc, HBRUSH hBrush) const {
		CHECK_PTR(lprc);
		CHECK_HANDLE(hBrush);

		int nResult = ::FillRect(m_hDC, lprc, hBrush);

		CHECK_NE_0(nResult);
	}
	void DrawIconEx(int xLeft, int yTop, HICON hIcon, int cxWidth, int cyWidth, UINT istepIfAniCur = 0, HBRUSH hbrFlickerFreeDraw = NULL, UINT diFlags = DI_NORMAL) const {
		BOOL fResult = ::DrawIconEx(m_hDC, xLeft, yTop, hIcon, cxWidth, cyWidth, istepIfAniCur, hbrFlickerFreeDraw, diFlags);

		CHECK(fResult);
	}
	HGDIOBJ SelectObject(HGDIOBJ hGdiObj) const {
		return ::SelectObject(m_hDC, hGdiObj);
	}
	HGDIOBJ SelectFont(HFONT hFont) const {
		CHECK_HANDLE(hFont);

		return ::SelectObject(m_hDC, hFont);
	}
	HGDIOBJ SelectPen(HPEN hPen) const {
		CHECK_HANDLE(hPen);

		return ::SelectObject(m_hDC, hPen);
	}
	void GetTextMetrics(LPTEXTMETRIC lptm) const {
		CHECK_PTR(lptm);

		BOOL fResult = ::GetTextMetrics(m_hDC, lptm);

		CHECK(fResult);
	}
	void DrawText(wstring szText, LPRECT lprc, UINT format) const {
		CHECK_PTR(lprc);

		int nResult = ::DrawText(m_hDC, szText.c_str(), szText.size(), lprc, format);

		CHECK_NE_0(nResult);
	}
	HDC GetHandle() const { return m_hDC; }
	void SetTextColor(COLORREF cr) const { ::SetTextColor(m_hDC, cr); }
	void SetBkColor(COLORREF cr) const { ::SetBkColor(m_hDC, cr); }
	void MoveToEx(INT x, INT y, LPPOINT lppt = NULL) const {
		BOOL fResult = ::MoveToEx(m_hDC, x, y, lppt);

		CHECK(fResult);
	}
	void LineTo(INT x, INT y) const {
		int nResult = ::LineTo(m_hDC, x, y);

		CHECK_NE_0(nResult);
	}
	void BitBlt(POINT ptDest, SIZE szDest, CDC * lpSourceDC, POINT ptSource, DWORD dwRop) const {
		ASSERT(lpSourceDC != NULL);

		BOOL fResult = ::BitBlt(m_hDC, ptDest.x, ptDest.y, szDest.cx, szDest.cy, lpSourceDC->GetHandle(), ptSource.x, ptSource.y, dwRop);

		CHECK(fResult);
	}
};

#endif // _INC_GDI
