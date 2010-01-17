#include "stdafx.h"
#include "include.h"

void CChromeFrameWebBrowser2::ExecuteUrlCallback(VARIANT * URL)
{
	if (m_lpFrame != NULL && URL != NULL && URL->vt == VT_BSTR)
	{
		m_lpFrame->ExecuteUrlCallback(URL->bstrVal);
	}
}
