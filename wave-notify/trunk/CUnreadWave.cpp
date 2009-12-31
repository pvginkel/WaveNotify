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

CUnreadWave::CUnreadWave(CWave * lpWave, CWaveMessage * lpMessage)
{
	m_szId = lpWave->GetID();
	m_szContact = lpMessage->GetEmailAddress();
	m_szSubject = lpWave->GetSubject();
	m_szBody = lpMessage->GetText();
	m_uUnread = lpWave->GetUnreadMessages();
	m_dtTime = lpWave->GetTime();
	m_lpContact = CNotifierApp::Instance()->GetWaveContact(m_szContact);
}

INT CUnreadWave::Paint(CDC & dc, RECT & rcClient, BOOL fMouseOver, BOOL fExcludeCloseButton) const
{
	static HFONT hBoldFont = CreateFontIndirectEx(GetMessageBoxFont(), FW_BOLD, FALSE, FALSE);
	static HFONT hUnderlineFont = CreateFontIndirectEx(GetMessageBoxFont(), FW_NORMAL, FALSE, TRUE);
	static HFONT hUnderlineBoldFont = CreateFontIndirectEx(GetMessageBoxFont(), FW_BOLD, FALSE, TRUE);

	wstring szContact = m_lpContact == NULL ? m_szContact : m_lpContact->GetDisplayName();

	//
	// Paint the From.
	//

	HFONT hSelectedFont;
	HFONT hSelectedBoldFont;

	if (fMouseOver)
	{
		dc.SetTextColor(RGB(0, 102, 204));
		hSelectedFont = hUnderlineFont;
		hSelectedBoldFont = hUnderlineBoldFont;
	}
	else
	{
		dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
		hSelectedFont = GetMessageBoxFont();
		hSelectedBoldFont = hBoldFont;
	}

	HGDIOBJ hOriginal = dc.SelectFont(hSelectedFont);

	RECT rc = rcClient;
	INT nUnreadWidth = 0;
	wstring szUnreadBuffer;

	//
	// Paint unread.
	//

	if (m_uUnread > 1)
	{
		szUnreadBuffer = Format(L" (%d)", m_uUnread);

		dc.DrawText(szUnreadBuffer, &rc, DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

		nUnreadWidth = rc.right - rc.left;
	}

	//
	// Paint the contact.
	//

	dc.SelectFont(hSelectedBoldFont);

	rc = rcClient;

	if (fExcludeCloseButton)
	{
		rc.right -= 20;
	}

	rc.right -= nUnreadWidth;

	dc.DrawText(szContact, &rc, DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	INT nOffset = rc.bottom + PL_LINE_SPACING;

	dc.DrawText(szContact, &rc, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	if (m_uUnread > 1)
	{
		dc.SelectFont(hSelectedFont);

		rc.left = rc.right;
		rc.right = rc.left + nUnreadWidth;

		dc.DrawText(szUnreadBuffer, &rc, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);
	}

	//
	// Paint the Subject
	//

	rc = rcClient;
	rc.top = nOffset;

	dc.DrawText(m_szSubject, &rc, DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);
	
	nOffset = rc.bottom;

	dc.DrawText(m_szSubject, &rc, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	//
	// Paint the Body
	//

	if (!m_szBody.empty())
	{
		nOffset += PL_LINE_SPACING;

		dc.SelectFont(hSelectedFont);

		TEXTMETRIC tm;

		dc.GetTextMetrics(&tm);

		rc = rcClient;
		rc.top = nOffset;

		dc.DrawText(m_szBody, &rc, DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_WORDBREAK);

		INT nFullHeight = ((rc.bottom - rc.top) / tm.tmHeight) * tm.tmHeight;
		INT nMaxHeight = ((rcClient.bottom - nOffset) / tm.tmHeight) * tm.tmHeight;

		rc.bottom = rc.top + min(nFullHeight, nMaxHeight);

		dc.DrawText(m_szBody, &rc, DT_END_ELLIPSIS | DT_NOPREFIX | DT_WORDBREAK);
	}

	dc.SelectObject(hOriginal);

	return rc.bottom - rcClient.top;
}

INT CUnreadWave::CalculateHeight(RECT & rcClient) const
{
	CDC dc(GetDC(NULL));

	static HFONT hBoldFont = CreateFontIndirectEx(GetMessageBoxFont(), FW_BOLD, FALSE, FALSE);

	HGDIOBJ hOriginal = dc.SelectFont(hBoldFont);

	TEXTMETRIC tm;

	dc.GetTextMetrics(&tm);

	INT nOffset = rcClient.top +
		tm.tmHeight + PL_LINE_SPACING + // Contact
		tm.tmHeight; // Subject

	if (!m_szBody.empty())
	{
		nOffset += PL_LINE_SPACING;

		dc.SelectFont(GetMessageBoxFont());

		dc.GetTextMetrics(&tm);

		RECT rc = rcClient;
		rc.top = nOffset;

		dc.DrawText(m_szBody, &rc, DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_WORDBREAK);

		INT nFullHeight = ((rc.bottom - rc.top) / tm.tmHeight) * tm.tmHeight;
		INT nMaxHeight = ((rcClient.bottom - nOffset) / tm.tmHeight) * tm.tmHeight;

		nOffset += min(nFullHeight, nMaxHeight);
	}

	dc.SelectObject(hOriginal);

	DeleteDC(dc.GetHandle());

	return nOffset - rcClient.top;
}
