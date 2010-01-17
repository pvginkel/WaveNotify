#include "stdafx.h"
#include "include.h"

CChromeFrameWindow::CChromeFrameWindow() :
	m_lpFrame(NULL),
	m_dwPropertySinkCookie(0),
	m_fCreated(FALSE)
{
}

void CChromeFrameWindow::ReadyStateChanged()
{
	if (m_lpFrame != NULL)
	{
		m_lpFrame->ReadyStateChanged();
	}
}

LRESULT CChromeFrameWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & fHandled)
{
	HRESULT hr;

	// Create the tab.

	m_vContainer.Attach(m_hWnd);

	hr = m_vContainer.CreateControl(CHROME_FRAME_PROGID);

	if (hr != ERROR_SUCCESS)
	{
		return -1;
	}

	// Set the client site.

	CComPtr<IOleObject> lpOleObject;

	hr = m_vContainer.QueryControl(&lpOleObject);

	if (hr != ERROR_SUCCESS)
	{
		return -1;
	}

	m_vClientSite.AddRef();

	hr = lpOleObject->SetClientSite(&m_vClientSite);

	if (hr != ERROR_SUCCESS)
	{
		m_vClientSite.Release();
		return -1;
	}

	hr = m_vContainer.QueryControl(&m_lpTab);

	if (hr != ERROR_SUCCESS)
	{
		m_vClientSite.Release();
		return -1;
	}

	// Link up the property sink.

	hr = AtlAdvise(m_lpTab, this, IID_IPropertyNotifySink, &m_dwPropertySinkCookie);

	if (hr != ERROR_SUCCESS)
	{
		m_lpTab.Release();
		m_vClientSite.Release();
		return -1;
	}

	m_fCreated = TRUE;

	return 0;
}

LRESULT CChromeFrameWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & fHandled)
{
	if (m_fCreated)
	{
		m_fCreated = FALSE;

		CComPtr<IOleObject> lpOleObject;

		HRESULT hr = m_vContainer.QueryControl(&lpOleObject);

		if (hr == ERROR_SUCCESS)
		{
			hr = lpOleObject->SetClientSite(NULL);
		}

		if (m_dwPropertySinkCookie != 0)
		{
			AtlUnadvise(m_lpTab, IID_IPropertyNotifySink, m_dwPropertySinkCookie);

			m_dwPropertySinkCookie = 0;
		}

		m_lpTab.Release();
	}

	return 0;
}

LRESULT CChromeFrameWindow::OnEraseBackgrond(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & fHandled)
{
	return 1;
}


HRESULT CChromeFrameWindow::InstallExtension(const wchar_t * szPath) { 
	BSTR bstrPath = SysAllocString(szPath);

	if (bstrPath == NULL)
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = m_lpTab->installExtension(bstrPath);

	SysFreeString(bstrPath);

	return hr;
}
HRESULT CChromeFrameWindow::PutSrc(const wchar_t * szUrl) {
	BSTR bstrUrl = SysAllocString(szUrl);

	if (bstrUrl == NULL)
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = m_lpTab->put_src(bstrUrl);

	SysFreeString(bstrUrl);

	return hr;
}
