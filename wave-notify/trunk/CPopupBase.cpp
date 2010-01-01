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

void CPopupBase::PaintBackground(CDC & dc)
{
	static HBRUSH hBorderBrush = CreateSolidBrush(RGB(237, 135, 0));

	static HICON hIcon = (HICON)LoadImage(
		CApp::Instance()->GetInstance(),
		MAKEINTRESOURCE(IDI_MAINICON),
		IMAGE_ICON, PL_ICON_SIZE, PL_ICON_SIZE, 0);
	
	static HBRUSH hBackgroundBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);

	static HICON hCloseButton = (HICON)LoadImage(
		CApp::Instance()->GetInstance(),
		MAKEINTRESOURCE(IDI_CLOSE_BUTTON),
		IMAGE_ICON, 16, 16, 0);

	RECT rc;

	GetClientRect(GetHandle(), &rc);

	for (INT i = 0; i < PL_BORDER_WIDTH; i++)
	{
		dc.FrameRect(&rc, hBorderBrush);
		InflateRect(&rc, -1, -1);
	}

	dc.FillRect(&rc, hBackgroundBrush);

	if (m_fPaintIcon)
	{
		dc.DrawIconEx(
			(PL_BORDER_WIDTH + PL_PADDING) + (PL_ICON_DX / 2),
			(PL_BORDER_WIDTH + PL_PADDING) + (PL_ICON_DY / 2),
			hIcon,
			PL_ICON_SIZE,
			PL_ICON_SIZE);
	}

	if (m_fEnableCloseButton)
	{
		m_rcCloseButtonRect.right = rc.right - 6;
		m_rcCloseButtonRect.top = 6;
		m_rcCloseButtonRect.left = m_rcCloseButtonRect.right - 16;
		m_rcCloseButtonRect.bottom = m_rcCloseButtonRect.top + 16;

		dc.DrawIconEx(
			m_rcCloseButtonRect.left,
			m_rcCloseButtonRect.top,
			hCloseButton,
			16,
			16);
	}
}

BOOL CPopupBase::HitTestCloseButton(POINT pt)
{
	if (m_fEnableCloseButton)
	{
		return PtInRect(&m_rcCloseButtonRect, pt);
	}
	else
	{
		return FALSE;
	}
}
