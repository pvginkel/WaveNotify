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

CUnreadWavePopup::CUnreadWavePopup(CUnreadWave * lpWave, UINT uIndex, UINT uCount) : CPopupBase(PT_WAVE)
{
	ASSERT(lpWave != NULL);

	m_lpWave = lpWave;
	m_uIndex = uIndex;
	m_uCount = uCount;

	SetWidth(PL_WIDTH);
	SetHeight(PL_WAVE_HEIGHT);
	SetDuration(3600);
}

CUnreadWavePopup::~CUnreadWavePopup()
{
	CNotifierApp::Instance()->GetAppWindow()->HaveReportedWave(m_lpWave->GetID());

	delete m_lpWave;
}

LRESULT CUnreadWavePopup::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_PAINT:
		return OnPaint();

	case WM_LBUTTONUP:
		return OnLeftButtonUp(lParam);

	case WM_POPUP_OPENING:
		GetWindow()->SetCursor(CNotifierApp::Instance()->GetCursorHand());
		return 0;

	default:
		return CPopupBase::WndProc(uMessage, wParam, lParam);
	}
}

LRESULT CUnreadWavePopup::OnPaint()
{
	PAINTSTRUCT ps;

	CDC dc(BeginPaint(GetHandle(), &ps));

	PaintBackground(dc);

	RECT rcClient;

	GetClientRect(GetHandle(), &rcClient);
	InflateRect(&rcClient, -(PL_BORDER_WIDTH + PL_PADDING), -(PL_BORDER_WIDTH + PL_PADDING));

	RECT rc = rcClient;
	rc.left += PL_TEXT_OFFSET;

	POINT pt = { -1, -1 };

	m_lpWave->Paint(dc, rc, FALSE, TRUE);

	//
	// Paint the Index and Count
	//

	if (m_uCount > 1)
	{
		HGDIOBJ hOriginal = dc.SelectObject(GetMessageBoxFont());

		rc = rcClient;
		rc.right = rc.left + PL_ICON_SIZE + PL_ICON_DX;
		rc.top += PL_ICON_SIZE + PL_ICON_DY + PL_LINE_SPACING;

		wstring szBuffer = Format(L"%d / %d", m_uIndex, m_uCount);

		dc.DrawText(szBuffer, &rc, DT_CENTER | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

		dc.SelectObject(hOriginal);
	}

	EndPaint(GetHandle(), &ps);

	return 0;
}

LRESULT CUnreadWavePopup::OnLeftButtonUp(LPARAM lParam)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if (HitTestCloseButton(pt))
	{
		GetWindow()->CancelAll();
	}
	else
	{
		CNotifierApp::Instance()->OpenUrl(
			CNotifierApp::Instance()->GetSession()->GetWaveUrl(m_lpWave->GetID()));
	}

	return 0;
}

void CUnreadWavePopup::UpdateUnread(CUnreadWave * lpUnread)
{
	ASSERT(lpUnread != NULL);

	delete m_lpWave;

	m_lpWave = lpUnread;

	if (IsDisplaying())
	{
		InvalidateRect(GetHandle(), NULL, FALSE);

		ExtendDuration(3600);
	}
}

void CUnreadWavePopup::ContactsUpdated(CWaveContactCollection * lpContacts)
{
	ASSERT(lpContacts != NULL);

	if (m_lpWave->GetWaveContact() == NULL)
	{
		CWaveContact * lpContact = lpContacts->GetContact(m_lpWave->GetContact());

		if (lpContact != NULL)
		{
			m_lpWave->SetWaveContact(lpContact);

			InvalidateRect(GetHandle(), NULL, FALSE);
		}
	}
}
