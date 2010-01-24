class CMozillaFrame : public CWindowHandle
{
private:
	LPMOZILLAFRAME m_lpHandle;

public:
	CMozillaFrame(CWindowHandle * lpParent, RECT & rc) {
		m_lpHandle = mozillaframe_create(lpParent->GetHandle(), &rc);
		if (m_lpHandle != NULL) {
			mozillaframe_set_param(m_lpHandle, this);
			mozillaframe_set_before_navigate_callback(m_lpHandle, BeforeNavigateCallback);
			SetHandle(mozillaframe_get_handle(m_lpHandle));
		}
	}
	~CMozillaFrame() {
		mozillaframe_destroy(m_lpHandle);
	}

	EventT2<wstring, LPBOOL> BeforeNavigate;

	BOOL IsCreated() { return m_lpHandle != NULL && IsWindow(); }
	BOOL Navigate(wstring szUrl) { return mozillaframe_navigate(m_lpHandle, szUrl.c_str()); }

protected:
	virtual void OnBeforeNavigate(wstring szUrl, LPBOOL lpCancel) {
		if (BeforeNavigate != NULL)
			BeforeNavigate(szUrl, lpCancel);
	}

private:
	static void CALLBACK BeforeNavigateCallback(LPMOZILLAFRAME lpHandle, LPCWSTR szUrl, LPBOOL fCancel) {
		((CMozillaFrame *)mozillaframe_get_param(lpHandle))->OnBeforeNavigate(szUrl, fCancel);
	}
};
