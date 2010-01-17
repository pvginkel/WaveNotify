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

HRESULT WINAPI CChromeFrameServiceProvider::GetInterface(void * pv, REFIID riid, LPVOID * ppv, DWORD dw)
{
	CChromeFrameServiceProvider * lpThis = (CChromeFrameServiceProvider*)pv;

	if (IsEqualIID(riid, IID_IChromeFramePrivileged))
		*ppv = &lpThis->m_vPriviliged;
	else if (
		IsEqualIID(riid, IID_IWebBrowser2) ||
		IsEqualIID(riid, IID_IWebBrowserApp) ||
		IsEqualIID(riid, IID_IWebBrowser) ||
		IsEqualIID(riid, IID_IDispatch)
	)
		*ppv = &lpThis->m_vWebBrowser2;
	else
		*ppv = NULL;

	return *ppv == NULL ? E_FAIL : S_OK;
}
