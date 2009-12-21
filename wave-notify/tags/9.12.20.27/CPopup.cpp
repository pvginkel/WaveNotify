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

CPopup::CPopup()
{
	m_nDuration = 5000;
	m_nHeight = 120;
	m_nWidth = 160;
	m_nAnimationDuration = 120;
	memset(&m_rcLocation, 0, sizeof(RECT));
}

void CPopup::Show()
{
	CPopupWindow::Instance(TRUE)->Show(this);
}

LRESULT CPopup::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	return CPopupWindow::Instance()->DefWindowProc(uMessage, wParam, lParam);
}

void CPopup::ExtendDuration(INT nDuration)
{
	if (IsDisplaying())
	{
		GetWindow()->ExtendPopupDuration(nDuration);
	}
	else
	{
		if (nDuration > m_nDuration)
		{
			m_nDuration = nDuration;
		}
	}
}
