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

CPropertySheet::CPropertySheet()
{
	m_dwFlags = 0;
	m_hIcon = NULL;
	m_szCaption = L"";
	m_uStartPage = 0;
	m_lpSheet = NULL;
}

CPropertySheet::~CPropertySheet()
{
	for (TPropertySheetPagesVectorIter iter = m_vPages.begin(); iter != m_vPages.end(); iter++)
	{
		if (*iter != NULL)
		{
			delete *iter;
		}
	}

	if (m_lpSheet != NULL)
	{
		if (m_lpSheet->ppsp != NULL)
		{
			free((void *)m_lpSheet->ppsp);
		}

		free(m_lpSheet);
	}
}

BOOL CPropertySheet::ShowDialog(CWindowHandle * lpParentWindow)
{
	m_lpSheet = BuildStructures(lpParentWindow);

	INT_PTR nResult = PropertySheet(m_lpSheet);

	return nResult > 0;
}

BOOL CPropertySheet::Create(INT nType, CWindowHandle * lpParentWindow)
{
	m_lpSheet = BuildStructures(lpParentWindow);

	m_lpSheet->dwFlags |= PSH_MODELESS;

	INT_PTR nResult = PropertySheet(m_lpSheet);

	if (nResult > 0)
	{
		SetHandle((HWND)nResult);

		CModelessPropertySheets::RegisterSheet(nType, this);

		OnCreated();

		ShowWindow(GetHandle(), SW_SHOW);
	}

	return nResult > 0;
}

LPPROPSHEETHEADER CPropertySheet::BuildStructures(CWindowHandle * lpParentWindow)
{
	LPPROPSHEETPAGE lpPages = (LPPROPSHEETPAGE)malloc(sizeof(PROPSHEETPAGE) * m_vPages.size());

	int i = 0;

	for (TPropertySheetPagesVectorIter iter = m_vPages.begin(); iter != m_vPages.end(); iter++, i++)
	{
		(*iter)->InitializeStructure(lpPages + i);
	}

	LPPROPSHEETHEADER lpSheet = (LPPROPSHEETHEADER)malloc(sizeof(PROPSHEETHEADER));

	memset(lpSheet, 0, sizeof(PROPSHEETHEADER));

	lpSheet->dwSize = sizeof(PROPSHEETHEADER);
	lpSheet->dwFlags = m_dwFlags | PSH_PROPSHEETPAGE | PSH_USEHICON | PSH_NOAPPLYNOW;
	lpSheet->hwndParent = lpParentWindow == NULL ? NULL : lpParentWindow->GetHandle();
	lpSheet->hInstance = CApp::Instance()->GetInstance();
	lpSheet->hIcon = m_hIcon;
	lpSheet->pszCaption = m_szCaption.c_str();
	lpSheet->nPages = m_vPages.size();
	lpSheet->ppsp = lpPages;

	return lpSheet;
}

void CPropertySheet::AddPage(CPropertySheetPage * lpPage)
{
	ASSERT(lpPage != NULL);

	m_vPages.push_back(lpPage);
}
