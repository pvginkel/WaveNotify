#ifndef _INC_CHATWINDOW
#define _INC_CHATWINDOW

#pragma once

#include "theming.h"
#include "notifierapp.h"
#include "mozillaframe.h"

class CChatLoadingWindow;

class CChatWindow : public CThemedWindow
{
private:
	wstring m_szUrl;
	wstring m_szWaveID;
	CMozillaFrame * m_lpMozillaFrame;
	CChatLoadingWindow * m_lpLoadingWindow;
	INT m_nUrlsSeen;

public:
	CChatWindow(wstring szUrl, wstring szWaveID);
	~CChatWindow();

protected:
	LRESULT OnCreate();
	LRESULT OnSize(SIZE szSize);

	ATOM CreateClass(LPWNDCLASSEX lpWndClass);
	HWND CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu);
	LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	void OnBeforeNavigate(wstring szUrl, LPBOOL fCancel);
	void OnNavigateComplete(wstring szUrl);
	void Perform();
};

class CChatLoadingWindow : public CWindow
{
public:
	CChatLoadingWindow() : CWindow(L"ChatLoading") { }

protected:
	ATOM CreateClass(LPWNDCLASSEX lpWndClass);
	HWND CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu);
	LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	LRESULT OnPaint();
	LRESULT OnSize(SIZE szSize);
};

#endif // _INC_CHATWINDOW
