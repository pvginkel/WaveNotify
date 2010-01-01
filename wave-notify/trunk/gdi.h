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
	CDC(HDC hDC) { m_hDC = hDC; }
	void FrameRect(LPRECT lprc, HBRUSH hBrush) const { ::FrameRect(m_hDC, lprc, hBrush); }
	void FillRect(LPRECT lprc, HBRUSH hBrush) const { ::FillRect(m_hDC, lprc, hBrush); }
	void DrawIconEx(int xLeft, int yTop, HICON hIcon, int cxWidth, int cyWidth, UINT istepIfAniCur = 0, HBRUSH hbrFlickerFreeDraw = NULL, UINT diFlags = DI_NORMAL) const {
		::DrawIconEx(m_hDC, xLeft, yTop, hIcon, cxWidth, cyWidth, istepIfAniCur, hbrFlickerFreeDraw, diFlags);
	}
	HGDIOBJ SelectObject(HGDIOBJ hGdiObj) const { return ::SelectObject(m_hDC, hGdiObj); }
	HGDIOBJ SelectFont(HFONT hFont) const { return ::SelectObject(m_hDC, hFont); }
	HGDIOBJ SelectPen(HPEN hPen) const { return ::SelectObject(m_hDC, hPen); }
	void GetTextMetrics(LPTEXTMETRIC lptm) const { ::GetTextMetrics(m_hDC, lptm); }
	void DrawText(wstring szText, LPRECT lprc, UINT format) const { ::DrawText(m_hDC, szText.c_str(), szText.size(), lprc, format); }
	HDC GetHandle() const { return m_hDC; }
	void SetTextColor(COLORREF cr) const { ::SetTextColor(m_hDC, cr); }
	void SetBkColor(COLORREF cr) const { ::SetBkColor(m_hDC, cr); }
	void MoveToEx(INT x, INT y, LPPOINT lppt = NULL) const { ::MoveToEx(m_hDC, x, y, lppt); }
	void LineTo(INT x, INT y) const { ::LineTo(m_hDC, x, y); }
	void BitBlt(POINT ptDest, SIZE szDest, CDC * lpSourceDC, POINT ptSource, DWORD dwRop) const {
		::BitBlt(m_hDC, ptDest.x, ptDest.y, szDest.cx, szDest.cy, lpSourceDC->GetHandle(), ptSource.x, ptSource.y, dwRop);
	}
};

#endif // _INC_GDI
