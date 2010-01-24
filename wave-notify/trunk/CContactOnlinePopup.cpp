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
#include "wave.h"
#include "popups.h"
#include "notifierapp.h"
#include "layout.h"

CContactOnlinePopup::CContactOnlinePopup(CWaveContact * lpContact, BOOL fOnline) : CPopupBase(PT_CONTACT_ONLINE)
{
	ASSERT(lpContact != NULL);

	m_lpContact = lpContact;
	m_fOnline = fOnline;

	SetHeight(PL_CO_HEIGHT);
	SetWidth(PL_CO_WIDTH);
	SetDuration(3600);
	SetPaintIcon(FALSE);
}

LRESULT CContactOnlinePopup::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_PAINT:
		return OnPaint();

	case WM_LBUTTONUP:
		return OnLeftButtonUp(lParam);

	case WM_POPUP_OPENING:
		GetWindow()->SetCursor(CNotifierApp::Instance()->GetCursorArrow());
		return 0;

	default:
		return CPopupBase::WndProc(uMessage, wParam, lParam);
	}
}

LRESULT CContactOnlinePopup::OnPaint()
{
	static HFONT hFont = CreateFontIndirectEx(GetMessageBoxFont(), FW_BOLD, FALSE, FALSE);

	CPaintDC dc(GetWindow());

	CAvatar * lpAvatar = m_lpContact->GetAvatar();

	if (lpAvatar == NULL)
	{
		lpAvatar = CNotifierApp::Instance()->GetGenericAvatar();

		ASSERT(lpAvatar != NULL);
	}

	PaintBackground(dc);

	POINT ptLocation = {
		PL_BORDER_WIDTH + PL_CO_ICON_DX,
		PL_BORDER_WIDTH + PL_CO_ICON_DY
	};

	lpAvatar->Paint(&dc, ptLocation);

	wstring szMessage(
		m_lpContact->GetDisplayName() +
		( m_fOnline ? L" is online." : L" went offline." )
	);

	RECT rcClient;
	
	GetWindow()->GetClientRect(&rcClient);
	InflateRect(&rcClient, -PL_BORDER_WIDTH, -PL_BORDER_WIDTH);

	rcClient.left += PL_CO_ICON_DX + PL_CO_ICON_SIZE + PL_CO_LABEL_DX;
	rcClient.top += PL_CO_ICON_DY + PL_CO_LABEL_DY;
	rcClient.right -= 16 + PL_PADDING;

	HGDIOBJ hOriginal = dc.SelectFont(hFont);

	TEXTMETRIC tm;

	dc.GetTextMetrics(&tm);

	dc.DrawText(szMessage, &rcClient, DT_NOPREFIX | DT_WORDBREAK);

	dc.SelectObject(hOriginal);

	return 0;
}

LRESULT CContactOnlinePopup::OnLeftButtonUp(LPARAM lParam)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if (HitTestCloseButton(pt))
	{
		GetWindow()->CancelAll();
	}

	return 0;
}
