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

CModelessDialogs::THandlesMap CModelessDialogs::m_vDialogs;

BOOL CModelessDialogs::IsDialogMessage(LPMSG lpMsg)
{
	for (THandlesMapIter iter = m_vDialogs.begin(); iter != m_vDialogs.end(); iter++)
	{
		if (::IsDialogMessage(iter->second->GetHandle(), lpMsg))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CModelessDialogs::RegisterDialog(INT nType, CDialog * lpDialog)
{
	if (ContainsType(nType))
	{
		FAIL("Dialog of type already registered");
	}

	m_vDialogs[nType] = lpDialog;
}

void CModelessDialogs::UnregisterDialog(INT nType)
{
	m_vDialogs.erase(nType);
}

void CModelessDialogs::UnregisterDialog(CDialog * lpDialog)
{
	for (THandlesMapIter iter = m_vDialogs.begin(); iter != m_vDialogs.end(); iter++)
	{
		if (iter->second == lpDialog)
		{
			m_vDialogs.erase(iter);
			return;
		}
	}
}

BOOL CModelessDialogs::ContainsType(INT nType)
{
	return m_vDialogs.find(nType) != m_vDialogs.end();
}
