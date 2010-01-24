#include "stdafx.h"
#include "include.h"
#include "chatwindow.h"

CChatWindow::CChatWindow(wstring szUrl, wstring szWaveID) :
	CThemedWindow(L"GoogleNotifierChatWindow"),
	m_szUrl(szUrl),
	m_szWaveID(szWaveID),
	m_lpMozillaFrame(NULL),
	m_lpLoadingWindow(NULL),
	m_nUrlsSeen(0)
{
	ASSERT(!szUrl.empty() && !szWaveID.empty());
}

CChatWindow::~CChatWindow()
{
	if (m_lpMozillaFrame != NULL)
	{
		delete m_lpMozillaFrame;
	}
}

ATOM CChatWindow::CreateClass(LPWNDCLASSEX lpWndClass)
{
	ASSERT(lpWndClass != NULL);

	lpWndClass->hIcon = CNotifierApp::Instance()->GetMainIcon();
	lpWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
	lpWndClass->hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	return CThemedWindow::CreateClass(lpWndClass);
}

HWND CChatWindow::CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu)
{
	return CThemedWindow::CreateHandle(
		dwExStyle,
		L"My First Chat Window",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		x, y, 375, 550,
		lpParentWindow,
		hMenu);
}

LRESULT CChatWindow::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_CREATE:
		return OnCreate();

	case WM_SIZE: {
		SIZE szSize = { LOWORD(lParam), HIWORD(lParam) };
		return OnSize(szSize);
	}
		      /*
	case WM_ERASEBKGND:
		return 1;
		*/
	}

	return CThemedWindow::WndProc(uMessage, wParam, lParam);
}

LRESULT CChatWindow::OnCreate()
{
	RECT rc;

	GetClientRect(&rc);

	m_lpLoadingWindow = new CChatLoadingWindow();

	m_lpLoadingWindow->Create(L"", this);
	m_lpLoadingWindow->ShowWindow(SW_SHOW);

	m_lpMozillaFrame = new CMozillaFrame(this, rc);

	if (!m_lpMozillaFrame->IsCreated())
	{
		LOG("Could not create mozilla frame");

		return -1;
	}

	m_lpMozillaFrame->BeforeNavigate += AddressOfT2<CChatWindow, wstring, LPBOOL>(this, &CChatWindow::OnBeforeNavigate);
	m_lpMozillaFrame->NavigateComplete += AddressOfT<CChatWindow, wstring>(this, &CChatWindow::OnNavigateComplete);
	
	m_lpMozillaFrame->Navigate(m_szUrl);

	return 0;
}

LRESULT CChatWindow::OnSize(SIZE szSize)
{
	ASSERT(m_lpMozillaFrame != NULL && m_lpLoadingWindow != NULL);

	RECT rt;

	GetClientRect(&rt);

	m_lpMozillaFrame->MoveWindow(0, 0, rt.right, rt.bottom);

	RECT rcLoading;

	m_lpLoadingWindow->GetClientRect(&rcLoading);
	m_lpLoadingWindow->MoveWindow(0, rt.bottom - rcLoading.bottom, rcLoading.right, rcLoading.bottom);

	return 0;
}

void CChatWindow::OnBeforeNavigate(wstring szUrl, LPBOOL lpCancel)
{
}

void CChatWindow::OnNavigateComplete(wstring szUrl)
{
	m_nUrlsSeen++;

	if (m_nUrlsSeen == 2)
	{
		m_lpLoadingWindow->ShowWindow(SW_HIDE);
	}
	else if (m_nUrlsSeen > 2)
	{
	}
}
