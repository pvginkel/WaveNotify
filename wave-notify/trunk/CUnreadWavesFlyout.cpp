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

CUnreadWavesFlyout::CUnreadWavesFlyout(CUnreadWaveCollection * lpWaves)
{
	m_lpWaves = lpWaves;

	SetWidth(FL_WIDTH);
	SetHeight(CalculateRects());

	CalculateInboxBounds();

	m_vLastHitTest = CHitTest(None);
}

CUnreadWavesFlyout::~CUnreadWavesFlyout()
{
	delete m_lpWaves;
}

LRESULT CUnreadWavesFlyout::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_PAINT:
		return OnPaint();

	case WM_MOUSEMOVE:
		return OnMouseMove(LOWORD(lParam), HIWORD(lParam));

	case WM_LBUTTONUP:
		return OnLeftButtonUp(LOWORD(lParam), HIWORD(lParam));

	case WM_MOUSELEAVE:
		return OnMouseMove(-1, -1);

	default:
		return CFlyout::WndProc(uMessage, wParam, lParam);
	}
}

LRESULT CUnreadWavesFlyout::OnPaint()
{
	PAINTSTRUCT ps;

	CDC dc(BeginPaint(GetHandle(), &ps));

	//
	// Paint the bottom
	//

	RECT rcClient;

	GetClientRect(GetHandle(), &rcClient);

	RECT rc = rcClient;
	rc.top = rc.bottom - FL_BOTTOM_HEIGHT;

	PaintBottom(dc, rc);

	rcClient.bottom -= FL_BOTTOM_HEIGHT;

	if (m_lpWaves->GetChanges().size() > 0)
	{
		PaintWaves(dc, rcClient);
	}
	else
	{
		PaintNoWaves(dc, rcClient);
	}

	EndPaint(GetHandle(), &ps);

	return 0;
}

void CUnreadWavesFlyout::PaintBottom(CDC & dc, RECT & rcBounds)
{
	static HPEN hBorderPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	static HBRUSH hBackgroundBrush = GetSysColorBrush(COLOR_BTNFACE);
	static HFONT hFont = CreateFontIndirectEx(GetMessageBoxFont(), FW_NORMAL, FALSE, TRUE);

	//
	// Draw the background
	//

	RECT rc = rcBounds;

	rc.top = rc.bottom - FL_BOTTOM_HEIGHT;

	HGDIOBJ hOriginal = dc.SelectPen(hBorderPen);

	dc.MoveToEx(rc.left, rc.top, NULL);
	dc.LineTo(rc.right, rc.top);

	dc.SelectObject(hOriginal);

	rc.top += 1;

	dc.FillRect(&rc, hBackgroundBrush);

	dc.SelectObject(hOriginal);

	//
	// Draw the label
	//

	hOriginal = dc.SelectFont(hFont);

	TEXTMETRIC tm;

	dc.GetTextMetrics(&tm);

	rc = rcBounds;
	rc.top = rc.bottom - FL_BOTTOM_HEIGHT + 1;

	INT nOffset = ((rc.bottom - rc.top) - tm.tmHeight) / 2;

	rc.top += nOffset;
	rc.left += nOffset;
	rc.right -= nOffset;

	dc.DrawText(L"Go to Inbox", &rc, DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	dc.SetTextColor(RGB(0, 0, 255));
	dc.SetBkColor(GetSysColor(COLOR_BTNFACE));

	dc.DrawText(L"Go to Inbox", &rc, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	INT nCount = m_lpWaves->GetChanges().size() - FL_MAXIMUM_WAVES;

	if (nCount > 0)
	{
		wstring szBuffer;

		if (nCount == 1)
		{
			szBuffer = L"1 more unread wave";
		}
		else
		{
			szBuffer = Format(L"%d more unread waves", nCount);
		}

		RECT rc2 = rc;
		rc2.left = rc2.right;
		rc2.right = rcBounds.right - nOffset;

		dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
		dc.SelectFont(GetMessageBoxFont());

		dc.DrawText(szBuffer, &rc2, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE | DT_RIGHT);
	}

	dc.SelectObject(hOriginal);
}

void CUnreadWavesFlyout::PaintWaves(CDC & dc, RECT & rcBounds)
{
	static HPEN hBorderPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));

	POINT pt;

	GetCursorPos(&pt);
	ScreenToClient(GetHandle(), &pt);

	CHitTest vHitTest = HitTest(pt);

	dc.SetBkColor(GetSysColor(COLOR_WINDOW));

	RECT rc = rcBounds;

	InflateRect(&rc, -PL_PADDING, -PL_PADDING);

	const TUnreadWaveVector & vWaves = m_lpWaves->GetChanges();

	HGDIOBJ hOriginal;

	UINT i = 0;

	for (TUnreadWaveVectorConstIter iter = vWaves.begin(); iter != vWaves.end(); iter++, i++)
	{
		CUnreadWave * lpWave = *iter;

		if (i > 0)
		{
			rc.top += FL_SPACING;

			hOriginal = dc.SelectPen(hBorderPen);

			dc.MoveToEx(rcBounds.left, rc.top);
			dc.LineTo(rcBounds.right, rc.top);

			dc.SelectObject(hOriginal);
			
			rc.top += FL_SPACING + 1;
		}

		BOOL fMouseOver = vHitTest.GetType() == UnreadWave && vHitTest.GetWave() == lpWave;

		RECT rcTmp = m_vRects[i];

		lpWave->Paint(dc, rcTmp, fMouseOver, FALSE);

		rc.top += rcTmp.bottom - rcTmp.top;

		if (i == FL_MAXIMUM_WAVES - 1)
		{
			break;
		}
	}
}

void CUnreadWavesFlyout::PaintNoWaves(CDC & dc, RECT & rcBounds)
{
	static HBRUSH hBackgroundBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);

	//
	// Draw the background
	//

	RECT r = rcBounds;
	r.bottom = r.top + FL_BOTTOM_HEIGHT;

	dc.FillRect(&r, hBackgroundBrush);

	//
	// Draw the label
	//

	TEXTMETRIC tm;

	memset(&tm, 0, sizeof(TEXTMETRIC));

	dc.GetTextMetrics(&tm);

	INT nOffset = ((r.bottom - r.top) - tm.tmHeight) / 2;

	r.top += nOffset;
	r.left += nOffset;
	r.right -= nOffset;

	dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
	dc.SetBkColor(GetSysColor(COLOR_WINDOW));

	HGDIOBJ hOriginal = dc.SelectFont(GetMessageBoxFont());

	dc.DrawText(L"There are no unread waves", &r, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	dc.SelectObject(hOriginal);
}

LRESULT CUnreadWavesFlyout::OnMouseMove(WORD x, WORD y)
{
	POINT pt = { x, y };

	CHitTest vHitTest = HitTest(pt);

	if (!vHitTest.Equals(m_vLastHitTest))
	{
		CDC dc(GetDC(GetHandle()));

		if (m_vLastHitTest.GetType() == UnreadWave)
		{
			CUnreadWave * lpWave = m_vLastHitTest.GetWave();

			RECT rcTmp = m_vRects[m_lpWaves->Find(lpWave)];

			lpWave->Paint(dc, rcTmp, FALSE, FALSE);
		}
		if (vHitTest.GetType() == UnreadWave)
		{
			CUnreadWave * lpWave = vHitTest.GetWave();

			RECT rcTmp = m_vRects[m_lpWaves->Find(lpWave)];

			lpWave->Paint(dc, rcTmp, TRUE, FALSE);
		}
	}

	if (m_vLastHitTest.GetType() != vHitTest.GetType())
	{
		SetCursor(
			vHitTest.GetType() == None ?
			CNotifierApp::Instance()->GetCursorArrow() :
			CNotifierApp::Instance()->GetCursorHand()
		);
	}

	m_vLastHitTest = vHitTest;

	return 0;
}

LRESULT CUnreadWavesFlyout::OnLeftButtonUp(WORD x, WORD y)
{
	POINT pt = { x, y };

	CHitTest vHitTest = HitTest(pt);

	switch (vHitTest.GetType())
	{
	case Inbox:
		OpenUrl(CNotifierApp::Instance()->GetSession()->GetInboxUrl());
		break;

	case UnreadWave:
		OpenUrl(CNotifierApp::Instance()->GetSession()->GetWaveUrl(vHitTest.GetWave()->GetID()));
		break;
	}

	return 0;
}

INT CUnreadWavesFlyout::CalculateRects()
{
	POINT pt = { -1, -1 };

	RECT rc = { 0, 0, FL_WIDTH - PL_PADDING * 2, 32267 };

	const TUnreadWaveVector & vWaves = m_lpWaves->GetChanges();

	INT i = 0;

	for (TUnreadWaveVectorConstIter iter = vWaves.begin(); iter != vWaves.end(); iter++, i++)
	{
		if (i > 0)
		{
			rc.top += FL_SPACING * 2 + 1;
		}

		RECT rcTmp = rc;

		INT nHeight = (*iter)->CalculateHeight(rcTmp);

		RECT rcWave = { PL_PADDING, PL_PADDING + rc.top, FL_WIDTH - PL_PADDING, PL_PADDING + rc.top + nHeight };

		m_vRects.push_back(rcWave);

		rc.top += nHeight;

		if (i == FL_MAXIMUM_WAVES - 1)
		{
			break;
		}
	}

	if (i == 0)
	{
		return FL_BOTTOM_HEIGHT * 2;
	}
	else
	{
		return rc.top + (PL_PADDING * 2) + FL_BOTTOM_HEIGHT;
	}
}

CUnreadWavesFlyout::CHitTest CUnreadWavesFlyout::HitTest(POINT pt) const
{
	for (UINT i = 0; i < m_vRects.size(); i++)
	{
		if (PtInRect(&m_vRects[i], pt))
		{
			return CHitTest(UnreadWave, m_lpWaves->GetChange(i));
		}
	}

	if (PtInRect(&m_rcInbox, pt))
	{
		return CHitTest(Inbox);
	}

	return CHitTest(None);
}

void CUnreadWavesFlyout::CalculateInboxBounds()
{
	CDC dc(GetDC(NULL));

	HGDIOBJ hOriginal = dc.SelectFont(GetMessageBoxFont());

	TEXTMETRIC tm;

	dc.GetTextMetrics(&tm);

	dc.SelectObject(hOriginal);

	DeleteDC(dc.GetHandle());

	INT nOffset = (FL_BOTTOM_HEIGHT - tm.tmHeight) / 2;

	INT nHeight = GetHeight();
	INT nWidth = GetWidth();

	m_rcInbox.bottom = nHeight - nOffset;
	m_rcInbox.top = m_rcInbox.bottom - tm.tmHeight;
	m_rcInbox.left = nOffset;
	m_rcInbox.right = nWidth - nOffset;

	InflateRect(&m_rcInbox, 2, 2);
}
