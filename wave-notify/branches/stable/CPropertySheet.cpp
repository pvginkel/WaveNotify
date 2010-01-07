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
		delete *iter;
	}

	if (m_lpSheet != NULL)
	{
		free((void *)m_lpSheet->ppsp);
		free(m_lpSheet);
	}
}

BOOL CPropertySheet::ShowDialog(CWindowHandle * lpParentWindow)
{
	BuildStructures(lpParentWindow);

	INT_PTR nResult = PropertySheet(m_lpSheet);

	return nResult > 0;
}

BOOL CPropertySheet::Create(INT nType, CWindowHandle * lpParentWindow)
{
	BuildStructures(lpParentWindow);

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

void CPropertySheet::BuildStructures(CWindowHandle * lpParentWindow)
{
	LPPROPSHEETPAGE lpPages = (LPPROPSHEETPAGE)malloc(sizeof(PROPSHEETPAGE) * m_vPages.size());

	int i = 0;

	for (TPropertySheetPagesVectorIter iter = m_vPages.begin(); iter != m_vPages.end(); iter++, i++)
	{
		(*iter)->InitializeStructure(lpPages + i);
	}

	m_lpSheet = (LPPROPSHEETHEADER)malloc(sizeof(PROPSHEETHEADER));

	memset(m_lpSheet, 0, sizeof(PROPSHEETHEADER));

	m_lpSheet->dwSize = sizeof(PROPSHEETHEADER);
	m_lpSheet->dwFlags = m_dwFlags | PSH_PROPSHEETPAGE | PSH_USEHICON | PSH_NOAPPLYNOW;
	m_lpSheet->hwndParent = lpParentWindow == NULL ? NULL : lpParentWindow->GetHandle();
	m_lpSheet->hInstance = CApp::Instance()->GetInstance();
	m_lpSheet->hIcon = m_hIcon;
	m_lpSheet->pszCaption = m_szCaption.c_str();
	m_lpSheet->nPages = m_vPages.size();
	m_lpSheet->ppsp = lpPages;
}

void CPropertySheet::AddPage(CPropertySheetPage * lpPage)
{
	m_vPages.push_back(lpPage);
}
