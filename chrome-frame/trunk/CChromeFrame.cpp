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

CChromeFrame::CChromeFrame(HWND hParent, LPRECT lpRect, LPCWSTR szSrc) :
	m_fCreated(FALSE),
	m_hParent(hParent),
	m_szUrl(szSrc),
	m_lpRect(lpRect),
	m_lpUrlCallback(NULL),
	m_lpNavigatedCallback(NULL),
	m_hWnd(NULL)
{
}

HWND CChromeFrame::Create()
{
	if (m_fCreated)
	{
		return NULL;
	}

	m_lpView = new CComObject<CChromeFrameWindow>();

	m_lpView->AddRef();
	m_lpView->SetFrame(this);

	HWND hWnd = m_lpView->Create(m_hParent, *m_lpRect, CHROME_FRAME_PROGID);

	if (hWnd != NULL)
	{
		m_fCreated = TRUE;
		m_hWnd = hWnd;

		return hWnd;
	}
	else
	{
		m_lpView->Release();

		return NULL;
	}
}

CChromeFrame::~CChromeFrame()
{
	if (m_fCreated && IsWindow(m_hWnd))
	{
		DestroyWindow(m_hWnd);

		m_lpView->Release();
	}
}

void CChromeFrame::ReadyStateChanged()
{
	long nReadyState;

	HRESULT hr = m_lpView->GetReadyState(&nReadyState);

	if (hr != ERROR_SUCCESS || nReadyState != READYSTATE_COMPLETE)
	{
		return;
	}

	// Add all extensions.

	for (TStringVectorIter iter = m_vExtensions.begin(); iter != m_vExtensions.end(); iter++)
	{
		m_lpView->InstallExtension(iter->c_str());
	}

	// Set the url we want to display.

	hr = m_lpView->PutSrc(m_szUrl.c_str());

	if (m_lpNavigatedCallback != NULL)
	{
		m_lpNavigatedCallback((LPCHROMEFRAME)this, hr == ERROR_SUCCESS);
	}
}
