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
