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

class CChromeFrame;

// TODO: For all output parameters of COM methods, they must be initialised even on failure.

class CChromeFramePriviliged :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IChromeFramePrivileged
{
public:
	BEGIN_COM_MAP(CChromeFramePriviliged)
		COM_INTERFACE_ENTRY(IChromeFramePrivileged)
	END_COM_MAP()

	STDMETHOD(GetWantsPrivileged)(boolean * wants_privileged) {
		*wants_privileged = true;
		return S_OK;
	}

	STDMETHOD(GetChromeExtraArguments)(BSTR *args) {
		*args = NULL;
		return E_FAIL;
	}

	STDMETHOD(GetChromeProfileName)(BSTR *profile_name) {
		*profile_name = NULL;
		return E_FAIL;
	}

	STDMETHOD(GetExtensionApisToAutomate)(BSTR *extension_apis) {
		*extension_apis = NULL;
		return E_FAIL;
	}
};

class CChromeFrameContainer :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IOleContainer
{
public:
	BEGIN_COM_MAP(CChromeFrameContainer)
		COM_INTERFACE_ENTRY(IOleContainer)
	END_COM_MAP()

        STDMETHOD(ParseDisplayName)(IBindCtx * pbc, LPOLESTR pszDisplayName, ULONG * pchEaten, IMoniker ** ppmkOut) {
		DEBUGBREAK; return E_NOTIMPL;
	}

	STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown ** ppenum) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(LockContainer)(BOOL fLock) {
		DEBUGBREAK; return E_NOTIMPL;
	}
};

class CChromeFrameWebBrowser2 :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IWebBrowser2, &IID_IWebBrowser2>
{
private:
	CChromeFrame * m_lpFrame;

public:
	CChromeFrameWebBrowser2() : m_lpFrame(NULL) { }

	void SetFrame(CChromeFrame * lpFrame) { m_lpFrame = lpFrame; }

	BEGIN_COM_MAP(CChromeFrameWebBrowser2)
		COM_INTERFACE_ENTRY(IWebBrowserApp)
		COM_INTERFACE_ENTRY(IWebBrowser2)
		COM_INTERFACE_ENTRY(IWebBrowser)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	// IWebBrowser2

        STDMETHOD(Navigate2)(VARIANT *URL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers) {
		ExecuteUrlCallback(URL);
		return S_OK;
	}
        
        STDMETHOD(QueryStatusWB)(OLECMDID cmdID, OLECMDF *pcmdf) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(ExecWB)(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(ShowBrowserBar)(VARIANT *pvaClsid, VARIANT *pvarShow, VARIANT *pvarSize) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_ReadyState)(READYSTATE *plReadyState) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Offline)(VARIANT_BOOL *pbOffline) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_Offline)(VARIANT_BOOL bOffline) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Silent)(VARIANT_BOOL *pbSilent) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_Silent)(VARIANT_BOOL bSilent) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_RegisterAsBrowser)(VARIANT_BOOL *pbRegister) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_RegisterAsBrowser)(VARIANT_BOOL bRegister) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_RegisterAsDropTarget)(VARIANT_BOOL *pbRegister) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_RegisterAsDropTarget)(VARIANT_BOOL bRegister) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_TheaterMode)(VARIANT_BOOL *pbRegister) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_TheaterMode)(VARIANT_BOOL bRegister) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_AddressBar)(VARIANT_BOOL *Value) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_AddressBar)(VARIANT_BOOL Value) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Resizable)(VARIANT_BOOL *Value) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_Resizable)(VARIANT_BOOL Value) {
		DEBUGBREAK; return E_NOTIMPL;
	}

	// IWebBrowserApp

	STDMETHOD(Quit)() {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(ClientToWindow)(int *pcx, int *pcy) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(PutProperty)(BSTR Property, VARIANT vtValue) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(GetProperty)(BSTR Property, VARIANT *pvtValue) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Name)(BSTR *Name) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_HWND)(SHANDLE_PTR *pHWND) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_FullName)(BSTR *FullName) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Path)(BSTR *Path) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Visible)(VARIANT_BOOL *pBool) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_Visible)(VARIANT_BOOL Value) {
		return S_OK;
	}
        
        STDMETHOD(get_StatusBar)(VARIANT_BOOL *pBool) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_StatusBar)(VARIANT_BOOL Value) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_StatusText)(BSTR *StatusText) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_StatusText)(BSTR StatusText) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_ToolBar)(int *Value) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_ToolBar)(int Value) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_MenuBar)(VARIANT_BOOL *Value) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_MenuBar)(VARIANT_BOOL Value) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_FullScreen)(VARIANT_BOOL *pbFullScreen) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_FullScreen)(VARIANT_BOOL bFullScreen) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
	// IWebBrowser

	STDMETHOD(GoBack)() {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(GoForward)() {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(GoHome)() {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(GoSearch)() {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(Navigate)(BSTR URL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(Refresh)() {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(Refresh2)(VARIANT *Level) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(Stop)() {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Application)(IDispatch **ppDisp) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Parent)(IDispatch **ppDisp) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Container)(IDispatch **ppDisp) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Document)(IDispatch **ppDisp) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_TopLevelContainer)(VARIANT_BOOL *pBool) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Type)(BSTR *Type) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Left)(long *pl) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_Left)(long Left) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Top)(long *pl) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_Top)(long Top) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Width)(long *pl) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_Width)(long Width) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Height)(long *pl) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(put_Height)(long Height) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_LocationName)(BSTR *LocationName) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_LocationURL)(BSTR *LocationURL) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(get_Busy)(VARIANT_BOOL *pBool) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
private:
	void ExecuteUrlCallback(VARIANT * URL);
};

class CChromeFrameServiceProvider :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IServiceProviderImpl<CChromeFrameServiceProvider>
{
private:
	CComObject<CChromeFramePriviliged> m_vPriviliged;
	CComObject<CChromeFrameWebBrowser2> m_vWebBrowser2;

public:
	void SetFrame(CChromeFrame * lpFrame) { m_vWebBrowser2.SetFrame(lpFrame); }

	BEGIN_COM_MAP(CChromeFrameServiceProvider)
		COM_INTERFACE_ENTRY(IServiceProvider)
		COM_INTERFACE_ENTRY_FUNC(IID_IChromeFramePrivileged, 0, GetInterface)
		COM_INTERFACE_ENTRY_FUNC(IID_IWebBrowserApp, 0, GetInterface)
		COM_INTERFACE_ENTRY_FUNC(IID_IWebBrowser2, 0, GetInterface)
		COM_INTERFACE_ENTRY_FUNC(IID_IWebBrowser, 0, GetInterface)
		COM_INTERFACE_ENTRY_FUNC(IID_IDispatch, 0, GetInterface)
	END_COM_MAP()

#if _MSC_VER <= 1200
#define InlineIsEqualGUID ::ATL::InlineIsEqualGUID
#endif

	BEGIN_SERVICE_MAP(CChromeFrameServiceProvider)
		SERVICE_ENTRY(IID_IChromeFramePrivileged)
		SERVICE_ENTRY(IID_IWebBrowserApp)
	END_SERVICE_MAP()

#if _MSC_VER <= 1200
#undef InlineIsEqualGUID
#endif

	static HRESULT WINAPI GetInterface(void * pv, REFIID riid, LPVOID * ppv, DWORD dw);
};

class CChromeFrameClientSite :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IOleClientSite
{
private:
	CComObject<CChromeFrameContainer> m_vContainer;
	CComObject<CChromeFrameServiceProvider> m_vServiceProvider;

public:
	void SetFrame(CChromeFrame * lpFrame) { m_vServiceProvider.SetFrame(lpFrame); }

	BEGIN_COM_MAP(CChromeFrameClientSite)
		COM_INTERFACE_ENTRY(IOleClientSite)
		COM_INTERFACE_ENTRY_FUNC(IID_IServiceProvider, 0, GetServiceProvider)
	END_COM_MAP()

	STDMETHOD(SaveObject)() {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) {
		DEBUGBREAK; return E_NOTIMPL;
	}
        
        STDMETHOD(GetContainer)(IOleContainer ** ppContainer) {
		*ppContainer = &m_vContainer;
		return S_OK;
	}
        
        STDMETHOD(ShowObject)() {
		return S_OK;
	}
        
        STDMETHOD(OnShowWindow)(BOOL fShow) {
		return S_OK;
	}
        
        STDMETHOD(RequestNewObjectLayout)() {
		DEBUGBREAK; return E_NOTIMPL;
	}

	static HRESULT WINAPI GetServiceProvider(void * pv, REFIID riid, LPVOID * ppv, DWORD dw) {
		*ppv = &((CChromeFrameClientSite*)pv)->m_vServiceProvider;
		return S_OK;
	}
};

class CChromeFrameWindow :
	public CWindowImpl<CChromeFrameWindow, CWindow, CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN> >,
	public CComObjectRootEx<CComSingleThreadModel>,
	public IPropertyNotifySink
{
private:
	CAxWindow m_vContainer;
	CChromeFrame * m_lpFrame;
	CComPtr<IChromeFrame> m_lpTab;
	CComObject<CChromeFrameClientSite> m_vClientSite;
	DWORD m_dwPropertySinkCookie;
	BOOL m_fCreated;

public:
	CChromeFrameWindow();
	~CChromeFrameWindow() { }

	void SetFrame(CChromeFrame * lpFrame) { m_lpFrame = lpFrame; }

	BEGIN_COM_MAP(CChromeFrameWindow)
		COM_INTERFACE_ENTRY(IPropertyNotifySink)
	END_COM_MAP()

	BEGIN_MSG_MAP(CChromeFrameWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackgrond)
	END_MSG_MAP()

	STDMETHOD(OnChanged)(DISPID dispID) {
		if (dispID == DISPID_READYSTATE)
			ReadyStateChanged();
		return S_OK;
	}
	STDMETHOD(OnRequestEdit)(DISPID dispID) {
		return S_OK;
	}

	HRESULT GetReadyState(long * lpReadyState) {
		return m_lpTab->get_readyState(lpReadyState);
	}
	HRESULT InstallExtension(const wchar_t * szPath);
	HRESULT PutSrc(const wchar_t * szUrl);

private:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & fHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & fHandled);
	LRESULT OnEraseBackgrond(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & fHandled);

	void ReadyStateChanged();
};

class CChromeFrame
{
private:
	BOOL m_fCreated;
	HWND m_hParent;
	wstring m_szUrl;
	CComObject<CChromeFrameWindow> * m_lpView;
	TStringVector m_vExtensions;
	RECT m_vRect;
	URLCALLBACK m_lpUrlCallback;
	NAVIGATEDCALLBACK m_lpNavigatedCallback;
	HWND m_hWnd;
	LPVOID m_lpParam;

public:
	CChromeFrame(HWND hParent, LPRECT lpRect, LPCWSTR szSrc);
	virtual ~CChromeFrame();

	HWND Create();
	void AddExtension(LPCWSTR szExtensionPath) {
		m_vExtensions.push_back(szExtensionPath);
	}
	void SetUrlCallback(URLCALLBACK lpCallback) {
		m_lpUrlCallback = lpCallback;
	}
	void SetNavigatedCallback(NAVIGATEDCALLBACK lpCallback) {
		m_lpNavigatedCallback = lpCallback;
	}
	void ExecuteUrlCallback(LPCWSTR szUrl) {
		if (m_lpUrlCallback != NULL)
			m_lpUrlCallback((LPCHROMEFRAME)this, szUrl);
	}
	HWND GetHandle() const {
		return m_fCreated ? m_hWnd : NULL;
	}
	void ReadyStateChanged();
	LPVOID GetParam() const { return m_lpParam; }
	void SetParam(LPVOID lpParam) { m_lpParam = lpParam; }
};
