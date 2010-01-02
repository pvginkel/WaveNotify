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

CModelessPropertySheets::THandlesMap CModelessPropertySheets::m_vSheets;

BOOL CModelessPropertySheets::IsDialogMessage(LPMSG lpMsg)
{
	for (THandlesMapIter iter = m_vSheets.begin(); iter != m_vSheets.end(); iter++)
	{
		CPropertySheet * lpSheet = iter->second;

		HWND hWnd = lpSheet->GetHandle();

		if (PropSheet_IsDialogMessage(hWnd, lpMsg))
		{
			if (PropSheet_GetCurrentPageHwnd(hWnd) == NULL)
			{
				DestroyWindow(hWnd);
				
				UnregisterSheet(iter->first);

				delete lpSheet;
			}

			return TRUE;
		}
	}

	return FALSE;
}

void CModelessPropertySheets::RegisterSheet(INT nType, CPropertySheet * lpSheet)
{
	if (ContainsType(nType))
	{
		FAIL("Dialog of type already registered");
	}

	m_vSheets[nType] = lpSheet;
}

void CModelessPropertySheets::UnregisterSheet(INT nType)
{
	m_vSheets.erase(nType);
}

void CModelessPropertySheets::UnregisterSheet(CPropertySheet * lpSheet)
{
	for (THandlesMapIter iter = m_vSheets.begin(); iter != m_vSheets.end(); iter++)
	{
		if (iter->second == lpSheet)
		{
			m_vSheets.erase(iter);
			return;
		}
	}
}

BOOL CModelessPropertySheets::ContainsType(INT nType)
{
	return m_vSheets.find(nType) != m_vSheets.end();
}
