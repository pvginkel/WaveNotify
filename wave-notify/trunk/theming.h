#ifndef _INC_THEMING
#define _INC_THEMING

#include "notifierapp.h"

typedef enum
{
	ThemeSchemeActive,
	ThemeSchemeActiveLight,
	ThemeSchemeInactive,
	ThemeSchemeInactiveLight,
	ThemeSchemeRed
} ThemeScheme;

typedef enum
{
	ThemeColorBorderBaseBottom,
	ThemeColorBorderBaseLeft,
	ThemeColorBorderBaseRight,
	ThemeColorBorderBaseTop,
	ThemeColorButtonBorderDarkBottom,
	ThemeColorButtonBorderDarkTop,
	ThemeColorButtonBorderLightBottom,
	ThemeColorButtonBorderLightTop,
	ThemeColorButtonDownBorderDarkBottom,
	ThemeColorButtonDownBorderDarkTop,
	ThemeColorButtonIconActive,
	ThemeColorButtonIconActiveBorder,
	ThemeColorButtonIconInactive,
	ThemeColorButtonIconInactiveBorder,
	ThemeColorClientBorder,
	ThemeColorGradientBottom,
	ThemeColorGradientButtonBottom,
	ThemeColorGradientDownTop,
	ThemeColorGradientDownButtonBottom,
	ThemeColorGradientTop,
	ThemeColorGrayBorderBottom,
	ThemeColorGrayBorderLeft,
	ThemeColorGrayBorderRight,
	ThemeColorGrayBorderTop,
	ThemeColorInnerBorderBottom,
	ThemeColorInnerBorderLeft,
	ThemeColorInnerBorderRight,
	ThemeColorInnerBorderTop,
	ThemeColorOuterBorderBottom,
	ThemeColorOuterBorderLeft,
	ThemeColorOuterBorderRight,
	ThemeColorOuterBorderTop,
	ThemeColorLoadingBorder,
	ThemeColorLoadingBackground,
	ThemeColorLoadingText
} ThemeColor;

typedef struct tagTHEMING_CALCS_BUTTON
{
	BOOL fVisible;
	RECT rcArea;
	BOOL fOver;
	BOOL fDown;
	BOOL fLeftMost;
	BOOL fRightMost;
} THEMING_CALCS_BUTTON;

typedef struct tagTHEMING_CALCS
{
	RECT rcArea;
	RECT rcButtonArea;
	RECT rcCaptionArea;
	RECT rcClientArea;
	BOOL fZoomed;
	BOOL fActive;
	DWORD dwCookie;
	THEMING_CALCS_BUTTON htMinimize;
	THEMING_CALCS_BUTTON htMaximize;
	THEMING_CALCS_BUTTON htClose;
} THEMING_CALCS;

class CThemeScheme : public CColorScheme
{
private:
	static CThemeScheme * m_lpSchemeActive;
	static CThemeScheme * m_lpSchemeActiveLight;
	static CThemeScheme * m_lpSchemeInactive;
	static CThemeScheme * m_lpSchemeInactiveLight;
	static CThemeScheme * m_lpSchemeRed;
	static HFONT m_hFont;
	static HFONT m_hBoldFont;

	HSBREF m_crBaseColor;

public:
	CThemeScheme(HSBREF crBaseColor) { m_crBaseColor = crBaseColor; }

	static void Cleanup();
	static CThemeScheme * GetScheme(ThemeScheme nScheme);
	static HFONT GetFont();
	static HFONT GetBoldFont();

	HSBREF GetHsbColor(INT nIndex);
};

class CThemeWindow : public CWindowHandle
{
private:
	BOOL m_fCapturing;
	INT m_nCapturingButton;
	BOOL m_fTracking;
	WNDPROC m_lpOldProc;
	DWORD m_dwLastDrawn;
	BOOL m_fInSizeMove;
	INT m_nRef;

private:
	CThemeWindow(HWND hWnd);
	virtual ~CThemeWindow();

public:
	static void Subclass(HWND hWnd);

private:
	LRESULT OnNcCalcSize(BOOL fCalculateClientArea, LPARAM lParam);
	LRESULT OnNcPaint(HRGN hRegion);
	LRESULT OnNcHitTest(POINT pt);
	LRESULT OnNcMouseLeave();
	LRESULT OnNcMouseMove(WPARAM wParam, POINT pt);
	LRESULT OnNcLButtonDown(WPARAM wParam, POINT pt);
	LRESULT OnLButtonUp(WPARAM wParam, POINT pt);
	LRESULT OnNcActivate(BOOL fActive);
	LRESULT OnSetText(LPCWSTR szText);
	LRESULT OnSetIcon(UINT nSizeType, HICON hIcon);

	static LRESULT CALLBACK WndProcCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void RecalculateRegion(INT nWidth, INT nHeight);
	void DrawFrame(CDC & dc, THEMING_CALCS & tc);
	void DrawButtons(CDC & dc, THEMING_CALCS & tcszSize);
	void DrawMaximizeIcon(CDC & dc, THEMING_CALCS & tc, THEMING_CALCS_BUTTON & tcn, CThemeScheme * lpScheme);
	void DrawMinimizeIcon(CDC & dc, THEMING_CALCS & tc, THEMING_CALCS_BUTTON & tcn, CThemeScheme * lpScheme);
	void DrawCloseIcon(CDC & dc, THEMING_CALCS & tc, THEMING_CALCS_BUTTON & tcn, CThemeScheme * lpScheme);
	void ConstructCalcs(THEMING_CALCS & tc);
	void ConstructCalcs(THEMING_CALCS & tc, POINT pt);
	void DrawButtonsFrame(CDC & dc, THEMING_CALCS & tc, CThemeScheme * lpScheme);
	void DrawButtonBackground(CDC & dc, THEMING_CALCS & tc, THEMING_CALCS_BUTTON & tcn, CThemeScheme * lpScheme);
	void GetWindowSize(SIZE & szSize);
	void ScreenToWindow(POINT & pt);
	void ClientToWindow(POINT & pt);
	void GetDrawingArea(RECT & rt);
	void DrawRestoredFrame(CDC & dc, THEMING_CALCS & tc, CThemeScheme * lpScheme);
	void DrawZoomedFrame(CDC & dc, THEMING_CALCS & tc, CThemeScheme * lpScheme);
	void DrawCaptionGradient(CDC & dc, RECT & rc, CThemeScheme * lpScheme);
	void PerformPaint();
	void AddRef() { m_nRef++; }
	void Release() {
		if (--m_nRef == 0)
			delete this;
	}
};

class CThemedDialog : public CDialog
{
protected:
	CThemedDialog(INT nResource) : CDialog(nResource) { }

	virtual void BeforeShow() {
		if (CNotifierApp::Instance()->GetEnableInlineChat())
			CThemeWindow::Subclass(GetHandle());
	}
};

class CThemedWindow : public CWindow
{
protected:
	CThemedWindow(wstring szClassName) : CWindow(szClassName) { }

	HWND CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu) {
		HWND hWnd = CWindow::CreateHandle(dwExStyle, szWindowName, dwStyle, x, y, cx, cy, lpParentWindow, hMenu);

		if (hWnd && CNotifierApp::Instance()->GetEnableInlineChat())
			CThemeWindow::Subclass(hWnd);

		return hWnd;
	}
};

#endif // _INC_THEMING
