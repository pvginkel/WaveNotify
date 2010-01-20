#include "stdafx.h"
#include "include.h"

_ATL_FUNC_INFO AFI_DWebBrowserEvents2_BeforeNavigate2 = {
	CC_STDCALL, VT_I4, 7, { VT_DISPATCH, VT_BSTR, VT_I4, VT_BSTR, VT_BYREF | VT_VARIANT, VT_BSTR, VT_BOOL | VT_BYREF }
};

static const CLSID CLSID_MozillaBrowser = { 0x1339B54C, 0x3453, 0x11D2, { 0x93, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

BOOL CMozillaFrame::Create(HWND hParent, LPRECT lpRect)
{
	if (m_fCreated)
	{
		return FALSE;
	}

	HWND hResult = m_vHost.Create(
		hParent, *lpRect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	if (hResult == NULL)
	{
		return FALSE;
	}

	// IExplore?

	// HRESULT hr = ::CoCreateInstance(__uuidof(WebBrowser), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUnknown), (void**)&m_lpControl);
	
	// Firefox (Prism)?

	HRESULT hr = ::CoCreateInstanceFromFile(
		L"mozctl.dll", CLSID_MozillaBrowser, NULL, __uuidof(IUnknown), (void**)&m_lpControl);

	// Attach the created control to our own window.

	if (SUCCEEDED(hr))
	{
		hr = m_vHost.AttachControl(m_lpControl, NULL);
	}

	if (SUCCEEDED(hr))
	{
		hr = AtlAdvise(m_lpControl, (IUnknown*)this, DIID_DWebBrowserEvents2, &m_dwAdviseSink);
	}

	// Get the web browser.

	if (SUCCEEDED(hr))
	{
		hr = m_lpControl->QueryInterface(IID_IWebBrowser2, (void**)&m_lpWebBrowser);

		if (SUCCEEDED(hr))
		{
			m_fCreated = TRUE;
		}
	}

	// Replace the client site with our own version so we can disable the context menu.

	CComPtr<IOleObject> lpOleObject;

	hr = m_lpWebBrowser->QueryInterface(IID_IOleObject, (void**)&lpOleObject);

	if (SUCCEEDED(hr))
	{
		CComPtr<IOleClientSite> lpClientSite;

		hr = lpOleObject->GetClientSite(&lpClientSite);

		if (SUCCEEDED(hr))
		{
			CComObject<CMozillaFrameClientSite> * lpNewClientSite =
				new CComObject<CMozillaFrameClientSite>();

			lpNewClientSite->SetParent(lpClientSite);

			hr = lpOleObject->SetClientSite(lpNewClientSite);

			if (FAILED(hr))
			{
				lpOleObject->SetClientSite(lpClientSite);

				lpNewClientSite->Release();
			}
		}
	}

	return m_fCreated;
}

void CMozillaFrame::Destroy()
{
	if(m_dwAdviseSink != 0xCDCDCDCD)
	{
		AtlUnadvise(m_lpControl, DIID_DWebBrowserEvents2, m_dwAdviseSink);

		m_dwAdviseSink = 0xCDCDCDCD;
	}

	m_lpWebBrowser.Release();
}

BOOL CMozillaFrame::NavigateTo(LPCWSTR szUrl)
{
	return SUCCEEDED(m_lpWebBrowser->Navigate(CComBSTR(szUrl), NULL, NULL, NULL, NULL));
}
