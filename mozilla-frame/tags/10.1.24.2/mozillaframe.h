#include "exdispid.h"

extern _ATL_FUNC_INFO AFI_DWebBrowserEvents2_BeforeNavigate2;
extern _ATL_FUNC_INFO AFI_DWebBrowserEvents2_NavigateComplete2;

// This interface is provided to allow disabling the context menu.
//
// See http://australia.proximity.on.ca/dxr/mozilla-central/embedding/browser/activex/src/control/MozillaBrowser.cpp.html
// for how this interface is used within the Mozilla control.

class CMozillaFrameClientSite :
	public IOleClientSite,
	public IDocHostUIHandler,
	public CComObjectRootEx<CComSingleThreadModel>
{
private:
	CComPtr<IOleClientSite> m_lpParent;

public:
	void SetParent(CComPtr<IOleClientSite> lpParent) { m_lpParent = lpParent; }

	BEGIN_COM_MAP(CMozillaFrameClientSite)
		COM_INTERFACE_ENTRY(IOleClientSite)
		COM_INTERFACE_ENTRY(IDocHostUIHandler)
		COM_INTERFACE_ENTRY_FUNC_BLIND(0, PerformQueryInterface)
	END_COM_MAP()

	// IOleClientSite

	STDMETHOD(SaveObject)() { return m_lpParent->SaveObject(); }
	STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) {
		return m_lpParent->GetMoniker(dwAssign, dwWhichMoniker, ppmk);
	}
	STDMETHOD(GetContainer)(IOleContainer **ppContainer) {
		return m_lpParent->GetContainer(ppContainer);
	}
	STDMETHOD(ShowObject)() { return m_lpParent->ShowObject(); }
	STDMETHOD(OnShowWindow)(BOOL fShow) { return m_lpParent->OnShowWindow(fShow); }
	STDMETHOD(RequestNewObjectLayout)() { return m_lpParent->RequestNewObjectLayout(); }

	// IDocHostUIHandler

	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved) {
		return S_OK; // E_NOTIMPL displays the default context menu; S_OK disables it
	}
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo) {
		return E_NOTIMPL;
	}
	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc) {
		return E_NOTIMPL;
	}
	STDMETHOD(HideUI)() {
		return E_NOTIMPL;
	}
	STDMETHOD(UpdateUI)() {
		return E_NOTIMPL;
	}
	STDMETHOD(EnableModeless)(BOOL fEnable) {
		return E_NOTIMPL;
	}
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate) {
		return E_NOTIMPL;
	}
	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate) {
		return E_NOTIMPL;
	}
	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow) {
		return E_NOTIMPL;
	}
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID) {
		return E_NOTIMPL;
	}
	STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw) {
		pchKey = NULL;
		return E_NOTIMPL;
	}
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget) {
		*ppDropTarget = NULL;
		return E_NOTIMPL;
	}
	STDMETHOD(GetExternal)(IDispatch **ppDispatch) {
		*ppDispatch = NULL;
		return E_NOTIMPL;
	}
	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut) {
		*ppchURLOut = NULL;
		return E_NOTIMPL;
	}
	STDMETHOD(FilterDataObject)( IDataObject *pDO, IDataObject **ppDORet) {
		*ppDORet = NULL;
		return E_NOTIMPL;
	}

private:
	static HRESULT WINAPI PerformQueryInterface(void * pv, REFIID riid, void ** ppv, DWORD dw) {
		return ((CMozillaFrameClientSite*)pv)->m_lpParent->QueryInterface(riid, ppv);
	}
};

// Why can't I define ATL_NO_VTABLE? (And why should I define it?)
class /*ATL_NO_VTABLE*/ CMozillaFrame :
	public IDispEventSimpleImpl<1, CMozillaFrame, &DIID_DWebBrowserEvents2>
{
private:
	BOOL m_fCreated;
	CAxWindow m_vHost;
	LPVOID m_lpParam;
	MFBEFORENAVIGATE m_lpBeforeNavigateCallback;
	MFNAVIGATECOMPLETE m_lpNavigateCompleteCallback;
	CComPtr<IWebBrowser2> m_lpWebBrowser;
	CComPtr<IUnknown> m_lpControl;
	DWORD m_dwAdviseSink;

public:
	CMozillaFrame() :
		m_fCreated(FALSE),
		m_lpParam(NULL),
		m_lpBeforeNavigateCallback(NULL),
		m_dwAdviseSink(0xCDCDCDCD) {
	}
	
	virtual ~CMozillaFrame() {
		if (m_fCreated)
			Destroy();
	}

	BEGIN_SINK_MAP(CMozillaFrame)
		SINK_ENTRY_INFO(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, DWebBrowserEvents2_BeforeNavigate2, &AFI_DWebBrowserEvents2_BeforeNavigate2)
		SINK_ENTRY_INFO(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, DWebBrowserEvents2_NavigateComplete2, &AFI_DWebBrowserEvents2_NavigateComplete2)
	END_SINK_MAP()

	BOOL Create(HWND hParent, LPRECT lpRect);
	void Destroy();
	BOOL NavigateTo(LPCWSTR szUrl);
	HWND GetHandle() const { return m_fCreated ? m_vHost.m_hWnd : NULL; }
	LPVOID GetParam() const { return m_lpParam; }
	void SetParam(LPVOID lpParam) { m_lpParam = lpParam; }
	void SetBeforeNavigateCallback(MFBEFORENAVIGATE lpCallback) { m_lpBeforeNavigateCallback = lpCallback; }
	void SetNavigateCompleteCallback(MFNAVIGATECOMPLETE lpCallback) { m_lpNavigateCompleteCallback = lpCallback; }

private:
	STDMETHOD(DWebBrowserEvents2_BeforeNavigate2)(IDispatch * lpDispatch, VARIANT * lpUrl, VARIANT * lpFlags, VARIANT * lpTargetFrameName, VARIANT * lpPostData, VARIANT * lpHeaders, VARIANT_BOOL * lpCancel)
	{
		if (m_lpBeforeNavigateCallback != NULL)
		{
			CComVariant vUrl(*lpUrl);
			BOOL fCancel = FALSE;

			vUrl.ChangeType(VT_BSTR);

			m_lpBeforeNavigateCallback((LPMOZILLAFRAME)this, vUrl.bstrVal, &fCancel);

			*lpCancel = fCancel ? VARIANT_TRUE : VARIANT_FALSE;
		}
		else
		{
			*lpCancel = VARIANT_FALSE;
		}

		return S_OK;
	}

	STDMETHOD(DWebBrowserEvents2_NavigateComplete2)(IDispatch * lpDispatch, VARIANT * lpUrl)
	{
		if (m_lpBeforeNavigateCallback != NULL)
		{
			CComVariant vUrl(*lpUrl);

			vUrl.ChangeType(VT_BSTR);

			m_lpNavigateCompleteCallback((LPMOZILLAFRAME)this, vUrl.bstrVal);
		}

		return S_OK;
	}
};
