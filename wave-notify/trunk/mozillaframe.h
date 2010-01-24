class CMozillaFrame : public CWindowHandle
{
private:
	LPMOZILLAFRAME m_lpHandle;

public:
	CMozillaFrame(CWindowHandle * lpParent, RECT & rc) {
		ASSERT(lpParent != NULL);

		m_lpHandle = mozillaframe_create(lpParent->GetHandle(), &rc);

		if (m_lpHandle != NULL) {
			mozillaframe_set_param(m_lpHandle, this);
			mozillaframe_set_before_navigate_callback(m_lpHandle, BeforeNavigateCallback);
			mozillaframe_set_navigate_complete_callback(m_lpHandle, NavigateCompleteCallback);
			SetHandle(mozillaframe_get_handle(m_lpHandle));
		}
	}
	~CMozillaFrame() {
		if (m_lpHandle != NULL)
			mozillaframe_destroy(m_lpHandle);
	}

	EventT2<wstring, LPBOOL> BeforeNavigate;
	EventT<wstring> NavigateComplete;

	BOOL IsCreated() { return m_lpHandle != NULL && IsWindow(); }
	BOOL Navigate(wstring szUrl) {
		ASSERT(m_lpHandle != NULL);
		return mozillaframe_navigate(m_lpHandle, szUrl.c_str());
	}

protected:
	virtual void OnBeforeNavigate(wstring szUrl, LPBOOL lpCancel) {
		if (BeforeNavigate != NULL)
			BeforeNavigate(szUrl, lpCancel);
	}
	virtual void OnNavigateComplete(wstring szUrl) {
		if (NavigateComplete != NULL)
			NavigateComplete(szUrl);
	}

private:
	static void CALLBACK BeforeNavigateCallback(LPMOZILLAFRAME lpHandle, LPCWSTR szUrl, LPBOOL fCancel) {
		ASSERT(lpHandle != NULL);

		((CMozillaFrame *)mozillaframe_get_param(lpHandle))->OnBeforeNavigate(szUrl, fCancel);
	}
	static void CALLBACK NavigateCompleteCallback(LPMOZILLAFRAME lpHandle, LPCWSTR szUrl) {
		ASSERT(lpHandle != NULL);

		((CMozillaFrame *)mozillaframe_get_param(lpHandle))->OnNavigateComplete(szUrl);
	}
};
