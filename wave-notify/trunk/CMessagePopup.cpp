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
#include "popups.h"
#include "notifierapp.h"
#include "layout.h"

CMessagePopup::CMessagePopup(wstring szMessage) : CPopupBase(PT_MESSAGE)
{
	CHECK_NOT_EMPTY(szMessage);

	m_szMessage = szMessage;

	SetHeight(PL_MSG_HEIGHT);
	SetWidth(PL_WIDTH);
	SetDuration(3000);
}

LRESULT CMessagePopup::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_PAINT:
		return OnPaint();

	case WM_LBUTTONUP:
		return OnLeftButtonUp(lParam);

	case WM_MOUSEMOVE:
		if (m_szUrl.empty())
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			GetWindow()->SetCursor(
				HitTestCloseButton(pt) ?
				CNotifierApp::Instance()->GetCursorHand() :
				CNotifierApp::Instance()->GetCursorArrow()
			);
		}
		return 0;

	case WM_POPUP_OPENING:
		GetWindow()->SetCursor(
			m_szUrl.empty() ?
			CNotifierApp::Instance()->GetCursorArrow() :
			CNotifierApp::Instance()->GetCursorHand()
		);
		return 0;

	default:
		return CPopupBase::WndProc(uMessage, wParam, lParam);
	}
}

LRESULT CMessagePopup::OnPaint()
{
	CPaintDC dc(GetWindow());

	PaintBackground(dc);

	RECT rcClient;
	
	GetClientRect(GetHandle(), &rcClient);
	InflateRect(&rcClient, -(PL_BORDER_WIDTH + PL_PADDING), -(PL_BORDER_WIDTH + PL_PADDING));

	rcClient.right -= 16;

	HGDIOBJ hOriginal = dc.SelectFont(GetMessageBoxFont());

	TEXTMETRIC tm;

	dc.GetTextMetrics(&tm);

	RECT rc = rcClient;
	rc.left += PL_TEXT_OFFSET;

	dc.DrawText(m_szMessage, &rc, DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK);

	INT nHeight = ((rc.bottom - rc.top) / tm.tmHeight) * tm.tmHeight;
	INT nOffset = ((rcClient.bottom - rcClient.top) - nHeight) / 2;
	INT nMinOffset = ((PL_ICON_SIZE + PL_ICON_DY) - tm.tmHeight) / 2;

	if (nOffset > nMinOffset)
	{
		nOffset = nMinOffset;
	}

	rc = rcClient;
	rc.left += PL_TEXT_OFFSET;
	rc.top += nOffset;
	rc.bottom = rc.top + nHeight;

	dc.DrawText(m_szMessage, &rc, DT_NOPREFIX | DT_WORDBREAK);

	dc.SelectObject(hOriginal);

	return 0;
}

LRESULT CMessagePopup::OnLeftButtonUp(LPARAM lParam)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if (HitTestCloseButton(pt))
	{
		GetWindow()->CancelAll();
	}
	else if (!m_szUrl.empty())
	{
		CNotifierApp::Instance()->OpenUrl(m_szUrl);
		GetWindow()->ShowNext();
	}

	return 0;
}
