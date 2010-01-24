#include "stdafx.h"
#include "include.h"
#include "theming.h"

#define PROPERTY_INSTANCE	L"WAVENOTIFY_THEME_INSTANCE"

#define HT_CORNER_MARGIN	16
#define HT_BORDER_WIDTH		5
#define HT_CAPTION_HEIGHT	26

#define LD_MINIMIZE		0x01
#define LD_MAXIMIZE		0x02
#define LD_CLOSE		0x03
#define LD_DOWN			0x04
#define LD_FORCE		0xff

// TODO: Do not theme for Wine ?!?

CThemeWindow::CThemeWindow(HWND hWnd) :
	CWindowHandle(hWnd),
	m_fCapturing(FALSE),
	m_fTracking(FALSE),
	m_nCapturingButton(0),
	m_dwLastDrawn(LD_FORCE),
	m_fInSizeMove(FALSE),
	m_nRef(1)
{
	// Subclass.

	m_lpOldProc = (WNDPROC)SetWindowLong(GWL_WNDPROC, (LONG)CThemeWindow::WndProcCallback);

	ASSERT(m_lpOldProc != NULL);

	SetProp(PROPERTY_INSTANCE, this);

	// Disable theming.

	Compat_SetWindowTheme(GetHandle(), L"", L"");

	// TODO: Do we want this?
	// DisableProcessWindowsGhosting();

	// Calculate the size we want to end with.

	RECT rc;
	POINT pt = { 0, 0 };

	GetClientRect(&rc);
	ClientToScreen(&pt);

	rc.left += pt.x;
	rc.top += pt.y;
	rc.right += pt.x;
	rc.bottom += pt.y;

	rc.left -= HT_BORDER_WIDTH;
	rc.right += HT_BORDER_WIDTH;
	rc.top -= HT_CAPTION_HEIGHT;
	rc.bottom += HT_BORDER_WIDTH;

	// Force a NC recalculation.

	SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
		SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE);

	// And then correct for the changed frame.
	// TODO: This seems the way to go, but it still isn't correct.

	SetWindowPos(NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
		SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	SIZE szSize;
	
	GetWindowSize(szSize);

	RecalculateRegion(szSize.cx + 1, szSize.cy + 1);
}

CThemeWindow::~CThemeWindow()
{
	::SetWindowLongPtr(GetHandle(), GWL_WNDPROC, (LONG_PTR)m_lpOldProc);

	::RemoveProp(GetHandle(), PROPERTY_INSTANCE);
}

void CThemeWindow::Subclass(HWND hWnd)
{
	// Constructor takes care of everything.

	new CThemeWindow(hWnd);
}

LRESULT CALLBACK CThemeWindow::WndProcCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CThemeWindow * lpWindow = (CThemeWindow*)::GetProp(hWnd, PROPERTY_INSTANCE);

	ASSERT(lpWindow != NULL);

	lpWindow->AddRef();

	LRESULT lResult = lpWindow->WndProc(uMsg, wParam, lParam);

	if (uMsg == WM_DESTROY)
	{
		lpWindow->Release();
	}

	lpWindow->Release();

	return lResult;
}

LRESULT CThemeWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCALCSIZE:
		return OnNcCalcSize(wParam, lParam);

	case WM_NCPAINT:
		return OnNcPaint((HRGN)wParam);

	case WM_NCHITTEST:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ScreenToWindow(pt);
			return OnNcHitTest(pt);
		}

	case WM_MOUSEMOVE:
		if (m_fCapturing)
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToWindow(pt);
			return OnNcMouseMove(wParam, pt);
		}
		break;

	case WM_NCMOUSEMOVE:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ScreenToWindow(pt);
			return OnNcMouseMove(wParam, pt);
		}

	case WM_NCMOUSELEAVE:
		return OnNcMouseLeave();

	case WM_NCLBUTTONDOWN:
		if (wParam == HTMINBUTTON || wParam == HTMAXBUTTON || wParam == HTCLOSE)
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ScreenToWindow(pt);
			return OnNcLButtonDown(wParam, pt);
		}
		break;

	case WM_LBUTTONUP:
		if (m_fCapturing)
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToWindow(pt);
			return OnLButtonUp(wParam, pt);
		}
		break;

	case WM_NCACTIVATE:
		return OnNcActivate((BOOL)wParam);

	case WM_ENTERSIZEMOVE:
		m_fInSizeMove = TRUE;
		break;

	case WM_EXITSIZEMOVE:
		m_fInSizeMove = FALSE;
		break;

	case WM_SETTEXT:
		return OnSetText((LPCWSTR)lParam);

	case WM_SETICON:
		return OnSetIcon(wParam, (HICON)lParam);
	}

	return CallWindowProc(m_lpOldProc, GetHandle(), uMsg, wParam, lParam);
}

void CThemeWindow::RecalculateRegion(INT nWidth, INT nHeight)
{
	HRGN hRegion;

	if (IsZoomed())
	{
		INT nBorder = GetSystemMetrics(SM_CXFRAME);

		hRegion = CreateRectRgn(
			nBorder,
			nBorder,
			nWidth - nBorder,
			nHeight - nBorder
		);
	}
	else
	{
		POINT vPoints[] = {
			{ 3, 0 },
			{ nWidth - 3, 0 },
			{ nWidth - 3, 1 },
			{ nWidth - 1, 1 },
			{ nWidth - 1, 3 },
			{ nWidth, 3 },
			{ nWidth, nHeight },
			{ 0, nHeight },
			{ 0, 3 },
			{ 1, 3 },
			{ 1, 1 },
			{ 3, 1 },
			{ 3, 0 }
		};

		hRegion = CreatePolygonRgn(vPoints, _ARRAYSIZE(vPoints), ALTERNATE);
	}

	SetWindowRgn(hRegion, TRUE);

	// TODO: Fix white dots.
	// PerformPaint();
}

LRESULT CThemeWindow::OnNcCalcSize(BOOL fCalculateClientArea, LPARAM lParam)
{
	if (fCalculateClientArea)
	{
		LPNCCALCSIZE_PARAMS lpParam = (LPNCCALCSIZE_PARAMS)lParam;

		ASSERT(lpParam != NULL);

		LPRECT lpClientNew = &lpParam->rgrc[0];
		LPRECT lpValidDst  = &lpParam->rgrc[1];
		LPRECT lpValidSrc  = &lpParam->rgrc[2];

		// Has the window size changed?

		if (
			((lpClientNew->right - lpClientNew->left) != (lpValidDst->right - lpValidDst->left)) ||
			((lpClientNew->bottom - lpClientNew->top) != (lpValidDst->bottom - lpValidDst->top))
		) {
			RecalculateRegion(lpClientNew->right - lpClientNew->left, lpClientNew->bottom - lpClientNew->top);
		}

		// Calculate the height of the menu.

		THEMING_CALCS tc;

		ConstructCalcs(tc);

		INT nBorderSize = tc.fZoomed ? GetSystemMetrics(SM_CXFRAME) : HT_BORDER_WIDTH;

		lpClientNew->left += nBorderSize;
		lpClientNew->right -= nBorderSize;
		lpClientNew->bottom -= nBorderSize;
		lpClientNew->top += tc.rcCaptionArea.bottom + 1;

		// TODO: Update the rects to force a minimum size.

		return 0;
	}

	return 0;
}

LRESULT CThemeWindow::OnNcPaint(HRGN hRegion)
{
	PerformPaint();

	return 0;
}

void CThemeWindow::DrawFrame(CDC & dc, THEMING_CALCS & tc)
{
	CThemeScheme * lpScheme = CThemeScheme::GetScheme(
		IsActiveWindow() ? ThemeSchemeActive : ThemeSchemeInactive);

	if (tc.fZoomed)
	{
		DrawZoomedFrame(dc, tc, lpScheme);
	}
	else
	{
		DrawRestoredFrame(dc, tc, lpScheme);
	}

	// Draw the text.

	HGDIOBJ hOriginal = dc.SelectFont(CThemeScheme::GetFont());

	wstring szText = GetWindowText();

	RECT rcText = { tc.rcArea.left, tc.rcArea.top, tc.rcButtonArea.left - 6, tc.rcCaptionArea.bottom };

	if (tc.fZoomed)
	{
		rcText.left += 1;
		rcText.top += 4;
	}
	else
	{
		rcText.left += 5;
		rcText.top += 6;
	}

	COLORREF crTextColor = dc.SetTextColor(RGB(255, 255, 255));
	INT nMode = dc.SetBkMode(TRANSPARENT);

	dc.DrawText(szText, &rcText, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	dc.SelectObject(hOriginal);
	dc.SetTextColor(crTextColor);
	dc.SetBkMode(nMode);

	// Draw the buttons.

	m_dwLastDrawn = LD_FORCE;

	DrawButtons(dc, tc);
}

void CThemeWindow::DrawCaptionGradient(CDC & dc, RECT & rc, CThemeScheme * lpScheme)
{
	ASSERT(lpScheme != NULL);

	// Draw the gradient.

	TRIVERTEX vert[2];
	GRADIENT_RECT gRect;
	COLORREF cr = lpScheme->GetColor(ThemeColorGradientTop);
	RGBQUAD cq = ColorRefToRGBQuad(cr);

	vert[0].x = rc.left;
	vert[0].y = rc.top;
	vert[0].Red = cq.rgbRed << 8;
	vert[0].Green = cq.rgbGreen << 8;
	vert[0].Blue = cq.rgbBlue << 8;
	vert[0].Alpha = 0;

	cr = lpScheme->GetColor(ThemeColorGradientBottom);
	cq = ColorRefToRGBQuad(cr);

	vert[1].x = rc.right;
	vert[1].y = rc.bottom;
	vert[1].Red = cq.rgbRed << 8;
	vert[1].Green = cq.rgbGreen << 8;
	vert[1].Blue = cq.rgbBlue << 8;
	vert[1].Alpha = 0;

	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;

	dc.GradientFill(vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}

void CThemeWindow::DrawRestoredFrame(CDC & dc, THEMING_CALCS & tc, CThemeScheme * lpScheme)
{
	ASSERT(lpScheme != NULL);

	RECT rcGradient = {
		tc.rcCaptionArea.left + 2,
		tc.rcCaptionArea.top + 2,
		tc.rcCaptionArea.right - 1,
		tc.rcCaptionArea.bottom - 2
	};

	DrawCaptionGradient(dc, rcGradient, lpScheme);

	// Draw the dark outer border.

	HGDIOBJ hOriginal = dc.SelectPen(lpScheme->GetPen(ThemeColorGrayBorderTop));

	dc.MoveToEx(tc.rcArea.left + 1, tc.rcArea.top + 3);
	dc.LineTo(tc.rcArea.left + 1, tc.rcArea.top + 1);
	dc.LineTo(tc.rcArea.left + 3, tc.rcArea.top + 1);
	dc.LineTo(tc.rcArea.left + 3, tc.rcArea.top);
	dc.LineTo(tc.rcArea.right - 3, tc.rcArea.top);

	dc.SelectPen(lpScheme->GetPen(ThemeColorGrayBorderRight));

	dc.LineTo(tc.rcArea.right - 3, tc.rcArea.top + 1);
	dc.LineTo(tc.rcArea.right - 1, tc.rcArea.top + 1);
	dc.LineTo(tc.rcArea.right - 1, tc.rcArea.top + 3);
	dc.LineTo(tc.rcArea.right - 0, tc.rcArea.top + 3);
	dc.LineTo(tc.rcArea.right, tc.rcArea.bottom);

	dc.SelectPen(lpScheme->GetPen(ThemeColorGrayBorderBottom));

	dc.LineTo(tc.rcArea.left, tc.rcArea.bottom);

	dc.SelectPen(lpScheme->GetPen(ThemeColorGrayBorderLeft));

	dc.LineTo(tc.rcArea.left, tc.rcArea.top + 3);
	dc.LineTo(tc.rcArea.left + 1, tc.rcArea.top + 3);

	// Draw the inner client border.

	dc.SelectPen(lpScheme->GetPen(ThemeColorClientBorder));

	dc.MoveToEx(tc.rcArea.left + 4, tc.rcCaptionArea.bottom);
	dc.LineTo(tc.rcArea.right - 4, tc.rcCaptionArea.bottom);
	dc.LineTo(tc.rcArea.right - 4, tc.rcArea.bottom - 4);
	dc.LineTo(tc.rcArea.left + 4, tc.rcArea.bottom - 4);
	dc.LineTo(tc.rcArea.left + 4, tc.rcCaptionArea.bottom);

	// Draw the outer border.

	dc.SelectPen(lpScheme->GetPen(ThemeColorOuterBorderTop));

	dc.MoveToEx(tc.rcArea.left + 2, tc.rcArea.top + 4);
	dc.LineTo(tc.rcArea.left + 2, tc.rcArea.top + 2);
	dc.LineTo(tc.rcArea.left + 4, tc.rcArea.top + 2);
	dc.LineTo(tc.rcArea.left + 4, tc.rcArea.top + 1);
	dc.LineTo(tc.rcArea.right - 4, tc.rcArea.top + 1);

	dc.SelectPen(lpScheme->GetPen(ThemeColorOuterBorderRight));

	dc.LineTo(tc.rcArea.right - 4, tc.rcArea.top + 1);
	dc.LineTo(tc.rcArea.right - 4, tc.rcArea.top + 2);
	dc.LineTo(tc.rcArea.right - 2, tc.rcArea.top + 2);
	dc.LineTo(tc.rcArea.right - 2, tc.rcArea.top + 4);
	dc.LineTo(tc.rcArea.right - 1, tc.rcArea.top + 4);
	dc.LineTo(tc.rcArea.right - 1, tc.rcArea.bottom - 1);

	dc.SelectPen(lpScheme->GetPen(ThemeColorOuterBorderBottom));

	dc.LineTo(tc.rcArea.left + 1, tc.rcArea.bottom - 1);

	dc.SelectPen(lpScheme->GetPen(ThemeColorOuterBorderLeft));

	dc.LineTo(tc.rcArea.left + 1, tc.rcArea.top + 3);

	// Draw the base border.

	dc.SelectPen(lpScheme->GetPen(ThemeColorBorderBaseTop));

	dc.MoveToEx(tc.rcArea.left + 2, tc.rcCaptionArea.bottom - 2);
	dc.LineTo(tc.rcArea.right - 2, tc.rcCaptionArea.bottom - 2);
	
	dc.SelectPen(lpScheme->GetPen(ThemeColorBorderBaseRight));

	dc.LineTo(tc.rcArea.right - 2, tc.rcArea.bottom - 2);
	
	dc.SelectPen(lpScheme->GetPen(ThemeColorBorderBaseBottom));

	dc.LineTo(tc.rcArea.left + 2, tc.rcArea.bottom - 2);

	dc.SelectPen(lpScheme->GetPen(ThemeColorBorderBaseLeft));

	dc.LineTo(tc.rcArea.left + 2, tc.rcCaptionArea.bottom - 2);

	// Draw the inner border.

	dc.SelectPen(lpScheme->GetPen(ThemeColorInnerBorderTop));

	dc.MoveToEx(tc.rcArea.left + 3, tc.rcCaptionArea.bottom - 1);
	dc.LineTo(tc.rcArea.right - 3, tc.rcCaptionArea.bottom - 1);
	
	dc.SelectPen(lpScheme->GetPen(ThemeColorInnerBorderRight));

	dc.LineTo(tc.rcArea.right - 3, tc.rcArea.bottom - 3);
	
	dc.SelectPen(lpScheme->GetPen(ThemeColorInnerBorderBottom));

	dc.LineTo(tc.rcArea.left + 3, tc.rcArea.bottom - 3);

	dc.SelectPen(lpScheme->GetPen(ThemeColorInnerBorderLeft));

	dc.LineTo(tc.rcArea.left + 3, tc.rcCaptionArea.bottom - 1);

	dc.SelectObject(hOriginal);
}

void CThemeWindow::DrawZoomedFrame(CDC & dc, THEMING_CALCS & tc, CThemeScheme * lpScheme)
{
	ASSERT(lpScheme != NULL);

	// Draw a blue border to fill up the hidden border.

	RECT rc;
	SIZE szSize;

	GetWindowSize(szSize);

	rc.left = 0;
	rc.top = 0;
	rc.right = tc.rcArea.left;
	rc.bottom = szSize.cy + 1;

	dc.FillRect(&rc, lpScheme->GetBrush(ThemeColorBorderBaseTop));

	rc.right = szSize.cx + 1;
	rc.left = tc.rcArea.right + 1;

	dc.FillRect(&rc, lpScheme->GetBrush(ThemeColorBorderBaseTop));

	rc.right = rc.left;
	rc.left = tc.rcArea.left;
	rc.top = 0;
	rc.bottom = tc.rcCaptionArea.top;

	dc.FillRect(&rc, lpScheme->GetBrush(ThemeColorBorderBaseTop));

	rc.bottom = szSize.cy + 1;
	rc.top = tc.rcArea.bottom + 1;

	dc.FillRect(&rc, lpScheme->GetBrush(ThemeColorBorderBaseTop));

	// Draw the caption bar.

	RECT rcGradient;

	memcpy(&rcGradient, &tc.rcCaptionArea, sizeof(RECT));

	rcGradient.bottom -= 2;
	rcGradient.right += 1;

	DrawCaptionGradient(dc, rcGradient, lpScheme);

	// Draw the inner client border.

	HGDIOBJ hOriginal = dc.SelectPen(lpScheme->GetPen(ThemeColorClientBorder));

	dc.MoveToEx(tc.rcArea.left, tc.rcCaptionArea.bottom);
	dc.LineTo(tc.rcArea.right + 1, tc.rcCaptionArea.bottom);

	// Draw the base border.

	dc.SelectPen(lpScheme->GetPen(ThemeColorBorderBaseTop));

	dc.MoveToEx(tc.rcArea.left, tc.rcCaptionArea.bottom - 2);
	dc.LineTo(tc.rcArea.right + 1, tc.rcCaptionArea.bottom - 2);

	// Draw the inner border.

	dc.SelectPen(lpScheme->GetPen(ThemeColorInnerBorderTop));

	dc.MoveToEx(tc.rcArea.left, tc.rcCaptionArea.bottom - 1);
	dc.LineTo(tc.rcArea.right + 1, tc.rcCaptionArea.bottom - 1);
	
	dc.SelectObject(hOriginal);
}

void CThemeWindow::DrawButtons(CDC & dc, THEMING_CALCS & tc)
{
	if (m_dwLastDrawn != LD_FORCE && m_dwLastDrawn == tc.dwCookie)
	{
		return;
	}

	m_dwLastDrawn = tc.dwCookie;

	CThemeScheme * lpScheme = CThemeScheme::GetScheme(
		tc.fActive ? ThemeSchemeActive : ThemeSchemeInactive);
	CThemeScheme * lpLightScheme = CThemeScheme::GetScheme(
		tc.fActive ? ThemeSchemeActiveLight : ThemeSchemeInactiveLight);
	CThemeScheme * lpRedScheme = CThemeScheme::GetScheme(ThemeSchemeRed);

	INT cx = tc.rcArea.right - tc.rcArea.left;
	INT cy = tc.rcArea.bottom - tc.rcArea.top;

	if (tc.htMinimize.fVisible)
	{
		DrawButtonBackground(dc, tc, tc.htMinimize, tc.htMinimize.fOver ? lpLightScheme : lpScheme);
		DrawMinimizeIcon(dc, tc, tc.htMinimize, lpScheme);
	}
	if (tc.htMaximize.fVisible)
	{
		DrawButtonBackground(dc, tc, tc.htMaximize, tc.htMaximize.fOver ? lpLightScheme : lpScheme);
		DrawMaximizeIcon(dc, tc, tc.htMaximize, lpScheme);
	}
	if (tc.htClose.fVisible)
	{
		DrawButtonBackground(dc, tc, tc.htClose, tc.htClose.fOver ? lpRedScheme : lpScheme);
		DrawCloseIcon(dc, tc, tc.htClose, lpScheme);
	}

	DrawButtonsFrame(dc, tc, lpScheme);
}

void CThemeWindow::DrawButtonBackground(CDC & dc, THEMING_CALCS & tc, THEMING_CALCS_BUTTON & tcn, CThemeScheme * lpScheme)
{
	ASSERT(lpScheme != NULL);

	RECT & rc = tc.rcButtonArea;

	// Draw the gradient.

	TRIVERTEX vert[2];
	GRADIENT_RECT gRect;
	COLORREF cr = lpScheme->GetColor(tcn.fDown ? ThemeColorGradientDownTop : ThemeColorGradientTop);
	RGBQUAD cq = ColorRefToRGBQuad(cr);

	vert[0].x = tcn.rcArea.left + 1;
	vert[0].y = rc.top + 2;
	vert[0].Red = cq.rgbRed << 8;
	vert[0].Green = cq.rgbGreen << 8;
	vert[0].Blue = cq.rgbBlue << 8;
	vert[0].Alpha = 0;

	cr = lpScheme->GetColor(tcn.fDown ? ThemeColorGradientDownButtonBottom : ThemeColorGradientButtonBottom);
	cq = ColorRefToRGBQuad(cr);

	vert[1].x = tcn.rcArea.right;
	vert[1].y = rc.bottom - 1;
	vert[1].Red = cq.rgbRed << 8;
	vert[1].Green = cq.rgbGreen << 8;
	vert[1].Blue = cq.rgbBlue << 8;
	vert[1].Alpha = 0;

	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;

	dc.GradientFill(vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
	
	// Draw the border.

	HGDIOBJ hOriginal = dc.SelectPen(lpScheme->GetPen(
		tcn.fDown ? ThemeColorButtonDownBorderDarkTop : ThemeColorButtonBorderLightTop));

	dc.MoveToEx(tcn.rcArea.left, rc.bottom - 2);
	dc.LineTo(tcn.rcArea.left, rc.top + 1);
	dc.LineTo(tcn.rcArea.right, rc.top + 1);

	dc.SelectObject(hOriginal);
}

void CThemeWindow::DrawButtonsFrame(CDC & dc, THEMING_CALCS & tc, CThemeScheme * lpScheme)
{
	ASSERT(lpScheme != NULL);

	// Draw the dark border.

	HGDIOBJ hOriginal = dc.SelectPen(lpScheme->GetPen(ThemeColorButtonBorderDarkTop));

	RECT & rc = tc.rcButtonArea;

	dc.MoveToEx(rc.left, rc.top + 1);
	dc.LineTo(rc.left, rc.bottom - 3);
	dc.LineTo(rc.left + 2, rc.bottom - 1);

	dc.SelectPen(lpScheme->GetPen(ThemeColorButtonBorderDarkBottom));

	dc.LineTo(rc.right - 3, rc.bottom - 1);
	dc.LineTo(rc.right - 1, rc.bottom - 3);
	dc.LineTo(rc.right - 1, rc.top);

	// Draw the light border.

	dc.SelectPen(lpScheme->GetPen(ThemeColorButtonBorderLightBottom));

	dc.MoveToEx(rc.left + 4, rc.bottom);
	dc.LineTo(rc.right - 3, rc.bottom);

	dc.LineTo(rc.right, rc.bottom - 3);
	dc.LineTo(rc.right, rc.top + 1);

	// Draw the lines in between.

	if (tc.htMinimize.fVisible)
	{
		dc.SelectPen(lpScheme->GetPen(ThemeColorButtonBorderDarkTop));

		dc.MoveToEx(tc.htMinimize.rcArea.right, rc.top + 1);
		dc.LineTo(tc.htMinimize.rcArea.right, rc.bottom - 1);
	}

	if (tc.htClose.fVisible)
	{
		dc.SelectPen(lpScheme->GetPen(ThemeColorButtonBorderDarkTop));

		dc.MoveToEx(tc.htClose.rcArea.left - 1, rc.top + 1);
		dc.LineTo(tc.htClose.rcArea.left - 1, rc.bottom - 1);
	}

	dc.SelectObject(hOriginal);
}

void CThemeWindow::DrawMinimizeIcon(CDC & dc, THEMING_CALCS & tc, THEMING_CALCS_BUTTON & tcn, CThemeScheme * lpScheme)
{
	ASSERT(lpScheme != NULL);

	HGDIOBJ hOriginal = dc.SelectPen(lpScheme->GetPen(
		tcn.fOver ? ThemeColorButtonIconActiveBorder : ThemeColorButtonIconInactiveBorder));

	RECT & rc = tcn.rcArea;

	// Draw the border.

	dc.MoveToEx(rc.left + 7, rc.top + 9);
	dc.LineTo(rc.right - 6, rc.top + 9);

	dc.MoveToEx(rc.right - 6, rc.top + 10);
	dc.LineTo(rc.right - 6, rc.top + 13);

	dc.MoveToEx(rc.right - 7, rc.top + 13);
	dc.LineTo(rc.left + 6, rc.top + 13);

	dc.MoveToEx(rc.left + 6, rc.top + 12);
	dc.LineTo(rc.left + 6, rc.top + 9);

	// Fill the icon.

	RECT rcFill;

	rcFill.left = rc.left + 7;
	rcFill.top = rc.top + 10;
	rcFill.right = rc.right - 6;
	rcFill.bottom = rc.top + 13;

	dc.FillRect(&rcFill, lpScheme->GetBrush(
		tcn.fOver ? ThemeColorButtonIconActive : ThemeColorButtonIconInactive));

	dc.SelectObject(hOriginal);
}

void CThemeWindow::DrawMaximizeIcon(CDC & dc, THEMING_CALCS & tc, THEMING_CALCS_BUTTON & tcn, CThemeScheme * lpScheme)
{
	ASSERT(lpScheme != NULL);

	HGDIOBJ hOriginal = dc.SelectPen(lpScheme->GetPen(
		tcn.fOver ? ThemeColorButtonIconActiveBorder : ThemeColorButtonIconInactiveBorder));

	RECT & rc = tcn.rcArea;

	// Draw the border.

	if (tc.fZoomed)
	{
		dc.MoveToEx(rc.left + 6, rc.top + 6);
		dc.LineTo(rc.right - 10, rc.top + 6);
		dc.LineTo(rc.right - 10, rc.bottom - 3);
		dc.MoveToEx(rc.left + 6, rc.bottom - 3);
		dc.LineTo(rc.right - 10, rc.bottom - 3);
		dc.MoveToEx(rc.left + 5, rc.top + 7);
		dc.LineTo(rc.left + 5, rc.bottom - 3);

		dc.MoveToEx(rc.left + 8, rc.top + 9);
		dc.LineTo(rc.right - 13, rc.top + 9);
		dc.LineTo(rc.right - 13, rc.top + 12);
		dc.LineTo(rc.left + 8, rc.top + 12);
		dc.LineTo(rc.left + 8, rc.top + 9);

		dc.MoveToEx(rc.left + 8, rc.top + 5);
		dc.LineTo(rc.left + 8, rc.top + 3);
		dc.MoveToEx(rc.left + 9, rc.top + 3);
		dc.LineTo(rc.right - 7, rc.top + 3);
		dc.MoveToEx(rc.right - 7, rc.top + 4);
		dc.LineTo(rc.right - 7, rc.bottom - 6);
		dc.MoveToEx(rc.right - 8, rc.bottom - 6);
		dc.LineTo(rc.right - 10, rc.bottom - 6);
	}
	else
	{
		dc.MoveToEx(rc.left + 7, rc.top + 4);
		dc.LineTo(rc.right - 7, rc.top + 4);
		dc.MoveToEx(rc.right - 7, rc.top + 5);
		dc.LineTo(rc.right - 7, rc.top + 13);
		dc.MoveToEx(rc.right - 8, rc.top + 13);
		dc.LineTo(rc.left + 6, rc.top + 13);
		dc.MoveToEx(rc.left + 6, rc.top + 5);
		dc.LineTo(rc.left + 6, rc.top + 13);

		dc.MoveToEx(rc.left + 9, rc.top + 7);
		dc.LineTo(rc.right - 10, rc.top + 7);
		dc.LineTo(rc.right - 10, rc.top + 10);
		dc.LineTo(rc.left + 9, rc.top + 10);
		dc.LineTo(rc.left + 9, rc.top + 7);
	}

	dc.SelectPen(lpScheme->GetPen(
		tcn.fOver ? ThemeColorButtonIconActive : ThemeColorButtonIconInactive));

	HBRUSH hBrush = lpScheme->GetBrush(
		tcn.fOver ? ThemeColorButtonIconActive : ThemeColorButtonIconInactive);

	if (tc.fZoomed)
	{
		dc.MoveToEx(rc.left + 9, rc.top + 4);
		dc.LineTo(rc.right - 7, rc.top + 4);
		dc.MoveToEx(rc.left + 9, rc.top + 5);
		dc.LineTo(rc.right - 7, rc.top + 5);
		dc.MoveToEx(rc.right - 8, rc.top + 6);
		dc.LineTo(rc.right - 8, rc.bottom - 6);
		dc.MoveToEx(rc.right - 9, rc.top + 6);
		dc.LineTo(rc.right - 9, rc.bottom - 6);

		RECT rcFrame = { rc.left + 6, rc.top + 7, rc.right - 10, rc.bottom - 3 };

		dc.FrameRect(&rcFrame, hBrush);
		InflateRect(&rcFrame, -1, -1);
		dc.FrameRect(&rcFrame, hBrush);
	}
	else
	{
		RECT rcFrame = { rc.left + 7, rc.top + 5, rc.right - 7, rc.top + 13 };

		dc.FrameRect(&rcFrame, hBrush);
		InflateRect(&rcFrame, -1, -1);
		dc.FrameRect(&rcFrame, hBrush);
	}

	dc.SelectObject(hOriginal);
}

void CThemeWindow::DrawCloseIcon(CDC & dc, THEMING_CALCS & tc, THEMING_CALCS_BUTTON & tcn, CThemeScheme * lpScheme)
{
	ASSERT(lpScheme != NULL);

	HGDIOBJ hOriginal = dc.SelectPen(lpScheme->GetPen(
		tcn.fOver ? ThemeColorButtonIconActiveBorder : ThemeColorButtonIconInactiveBorder));

	RECT & rc = tcn.rcArea;

	dc.MoveToEx(rc.left + 15, rc.top + 5);
	dc.LineTo(rc.left + 16, rc.top + 4);
	dc.LineTo(rc.left + 18, rc.top + 4);
	dc.LineTo(rc.left + 20, rc.top + 6);
	dc.LineTo(rc.left + 22, rc.top + 4);
	dc.LineTo(rc.left + 24, rc.top + 4);
	dc.LineTo(rc.left + 25, rc.top + 5);
	dc.LineTo(rc.left + 22, rc.top + 8);
	dc.LineTo(rc.left + 22, rc.top + 9);
	dc.LineTo(rc.left + 25, rc.top + 12);
	dc.LineTo(rc.left + 24, rc.top + 13);
	dc.LineTo(rc.left + 22, rc.top + 13);
	dc.LineTo(rc.left + 20, rc.top + 11);
	dc.LineTo(rc.left + 18, rc.top + 13);
	dc.LineTo(rc.left + 16, rc.top + 13);
	dc.LineTo(rc.left + 15, rc.top + 12);
	dc.LineTo(rc.left + 18, rc.top + 9);
	dc.LineTo(rc.left + 18, rc.top + 8);
	dc.LineTo(rc.left + 15, rc.top + 5);

	dc.SelectPen(lpScheme->GetPen(
		tcn.fOver ? ThemeColorButtonIconActive : ThemeColorButtonIconInactive));

	dc.MoveToEx(rc.left + 16, rc.top + 5);
	dc.LineTo(rc.left + 23, rc.top + 13);
	dc.MoveToEx(rc.left + 17, rc.top + 5);
	dc.LineTo(rc.left + 24, rc.top + 13);
	dc.MoveToEx(rc.left + 18, rc.top + 5);
	dc.LineTo(rc.left + 25, rc.top + 13);

	dc.MoveToEx(rc.left + 16, rc.top + 12);
	dc.LineTo(rc.left + 23, rc.top + 4);
	dc.MoveToEx(rc.left + 17, rc.top + 12);
	dc.LineTo(rc.left + 24, rc.top + 4);
	dc.MoveToEx(rc.left + 18, rc.top + 12);
	dc.LineTo(rc.left + 25, rc.top + 4);

	dc.SelectObject(hOriginal);
}

void CThemeWindow::ConstructCalcs(THEMING_CALCS & tc)
{
	POINT pt;

	GetCursorPos(&pt);
	ScreenToWindow(pt);

	return ConstructCalcs(tc, pt);
}

void CThemeWindow::ConstructCalcs(THEMING_CALCS & tc, POINT pt)
{
	memset(&tc, 0, sizeof(tc));

	tc.fZoomed = IsZoomed();
	tc.fActive = IsActiveWindow();

	GetDrawingArea(tc.rcArea);

	INT cx = tc.rcArea.right - tc.rcArea.left;
	INT cy = tc.rcArea.bottom - tc.rcArea.top;

	tc.htMinimize.fVisible = HasMinimizeButton();
	tc.htMaximize.fVisible = HasMaximizeButton();
	tc.htClose.fVisible = HasCloseButton();

	if (!tc.htMinimize.fVisible && !tc.htMaximize.fVisible && !tc.htClose.fVisible)
	{
		return;
	}

	tc.rcButtonArea.top = tc.rcArea.top + ( tc.fZoomed ? -1 : 0 );
	tc.rcButtonArea.bottom = tc.rcButtonArea.top + 18;
	tc.rcButtonArea.right = tc.rcArea.right - ( tc.fZoomed ? 2 : 4 );

	INT nLeft = tc.rcButtonArea.right - 1;

	tc.rcCaptionArea.left = tc.rcArea.left;
	tc.rcCaptionArea.right = tc.rcArea.right;
	tc.rcCaptionArea.top = tc.rcArea.top;
	tc.rcCaptionArea.bottom = tc.rcArea.top + ( tc.fZoomed ? 20 : 25 );

	memcpy(&tc.rcClientArea, &tc.rcArea, sizeof(RECT));

	tc.rcClientArea.top = tc.rcCaptionArea.bottom + 1;

	if (!tc.fZoomed)
	{
		tc.rcClientArea.left += HT_BORDER_WIDTH;
		tc.rcClientArea.bottom -= HT_BORDER_WIDTH - 1;
		tc.rcClientArea.right -= HT_BORDER_WIDTH - 1;
	}

	if (tc.htClose.fVisible)
	{
		memcpy(&tc.htClose.rcArea, &tc.rcButtonArea, sizeof(RECT));

		tc.htClose.rcArea.right = nLeft;
		nLeft -= 41;
		tc.htClose.rcArea.left = nLeft;
		nLeft--;

		tc.htClose.fOver = PtInRect(&tc.htClose.rcArea, pt);

		if (m_fInSizeMove || (m_fCapturing && m_nCapturingButton != HTCLOSE))
		{
			tc.htClose.fOver = FALSE;
		}

		tc.htClose.fDown = tc.htClose.fOver && m_fCapturing;

		if (tc.htClose.fOver)
		{
			tc.dwCookie = LD_CLOSE | ( tc.htClose.fDown ? LD_DOWN : 0 );
		}

		tc.htClose.fRightMost = TRUE;
		tc.htClose.fLeftMost = !tc.htMinimize.fVisible && !tc.htMaximize.fVisible;
	}

	if (tc.htMaximize.fVisible)
	{
		memcpy(&tc.htMaximize.rcArea, &tc.rcButtonArea, sizeof(RECT));

		tc.htMaximize.rcArea.right = nLeft;
		nLeft -= 24;
		tc.htMaximize.rcArea.left = nLeft;
		nLeft--;

		tc.htMaximize.fOver = PtInRect(&tc.htMaximize.rcArea, pt);

		if (m_fInSizeMove || (m_fCapturing && m_nCapturingButton != HTMAXBUTTON))
		{
			tc.htMaximize.fOver = FALSE;
		}

		tc.htMaximize.fDown = tc.htMaximize.fOver && m_fCapturing;

		if (tc.htMaximize.fOver)
		{
			tc.dwCookie = LD_MAXIMIZE | ( tc.htMaximize.fDown ? LD_DOWN : 0 );
		}

		tc.htMaximize.fRightMost = !tc.htClose.fVisible;
		tc.htMaximize.fLeftMost = !tc.htMinimize.fVisible;
	}

	if (tc.htMinimize.fVisible)
	{
		memcpy(&tc.htMinimize.rcArea, &tc.rcButtonArea, sizeof(RECT));

		tc.htMinimize.rcArea.right = nLeft;
		nLeft -= 24;
		tc.htMinimize.rcArea.left = nLeft;
		nLeft--;

		tc.htMinimize.fOver = PtInRect(&tc.htMinimize.rcArea, pt);

		if (m_fInSizeMove || (m_fCapturing && m_nCapturingButton != HTMINBUTTON))
		{
			tc.htMinimize.fOver = FALSE;
		}

		tc.htMinimize.fDown = tc.htMinimize.fOver && m_fCapturing;

		if (tc.htMinimize.fOver)
		{
			tc.dwCookie = LD_MINIMIZE | ( tc.htMinimize.fDown ? LD_DOWN : 0 );
		}

		tc.htMinimize.fRightMost = !tc.htClose.fVisible && !tc.htMaximize.fVisible;
		tc.htMinimize.fLeftMost = TRUE;
	}

	tc.rcButtonArea.left = nLeft;
}

LRESULT CThemeWindow::OnNcHitTest(POINT pt)
{
	THEMING_CALCS tc;

	ConstructCalcs(tc, pt);

	RECT rcClient;
	POINT ptClientOffset = { 0, 0 };

	GetClientRect(&rcClient);
	ClientToWindow(ptClientOffset);

	rcClient.left += ptClientOffset.x;
	rcClient.top += ptClientOffset.y;
	rcClient.right += ptClientOffset.x;
	rcClient.bottom += ptClientOffset.y;

	// Verify we're outside the screen area.

	if (tc.fZoomed && !PtInRect(&tc.rcArea, pt))
	{
		return HTNOWHERE;
	}

	// Verify whether we're over one of the buttons.

	if (tc.htMinimize.fOver)
	{
		return HTMINBUTTON;
	}
	else if (tc.htMaximize.fOver)
	{
		return HTMAXBUTTON;
	}
	else if (tc.htClose.fOver)
	{
		return HTCLOSE;
	}

	// Verify we're over the border (only when not maximized).

	BOOL fSizable = !!(GetWindowLong(GWL_STYLE) & WS_THICKFRAME);

	if (!tc.fZoomed & fSizable)
	{
		if (pt.y < tc.rcCaptionArea.top + HT_BORDER_WIDTH)
		{
			if (pt.x < rcClient.left + HT_CORNER_MARGIN)
			{
				return HTTOPLEFT;
			}
			else if (pt.x > rcClient.right - HT_CORNER_MARGIN)
			{
				return HTTOPRIGHT;
			}
			else
			{
				return HTTOP;
			}
		}
		else if (pt.y > rcClient.bottom - HT_BORDER_WIDTH)
		{
			if (pt.x < rcClient.left + HT_CORNER_MARGIN)
			{
				return HTBOTTOMLEFT;
			}
			else if (pt.x > rcClient.right - HT_CORNER_MARGIN)
			{
				return HTBOTTOMRIGHT;
			}
			else
			{
				return HTBOTTOM;
			}
		}
		else if (pt.x < rcClient.left + HT_BORDER_WIDTH)
		{
			if (pt.y < tc.rcCaptionArea.top + HT_CORNER_MARGIN)
			{
				return HTTOPLEFT;
			}
			else if (pt.y > rcClient.bottom - HT_CORNER_MARGIN)
			{
				return HTBOTTOMLEFT;
			}
			else
			{
				return HTLEFT;
			}
		}
		else if (pt.x > rcClient.right - HT_BORDER_WIDTH)
		{
			if (pt.y < tc.rcCaptionArea.top + HT_CORNER_MARGIN)
			{
				return HTTOPRIGHT;
			}
			else if (pt.y > rcClient.bottom - HT_CORNER_MARGIN)
			{
				return HTBOTTOMRIGHT;
			}
			else
			{
				return HTRIGHT;
			}
		}
	}

	// Verify we're in the caption bar.

	if (pt.y < rcClient.top)
	{
		return HTCAPTION;
	}

	// Check whether we're on a border when we're not sizable.

	if (!tc.fZoomed && !fSizable)
	{
		if (!PtInRect(&rcClient, pt))
		{
			return HTBORDER;
		}
	}

	// Otherwise, we're in the client area.

	return HTCLIENT;
}

void CThemeWindow::GetWindowSize(SIZE & szSize)
{
	RECT rc;

	GetWindowRect(&rc);

	szSize.cx = rc.right - rc.left - 1;
	szSize.cy = rc.bottom - rc.top - 1;
}

void CThemeWindow::ScreenToWindow(POINT & pt)
{
	RECT rc;

	GetWindowRect(&rc);
	POINT ptWindow = { rc.left, rc.top };
	ScreenToClient(&ptWindow);
	ScreenToClient(&pt);

	pt.x -= ptWindow.x;
	pt.y -= ptWindow.y;
}

void CThemeWindow::ClientToWindow(POINT & pt)
{
	ClientToScreen(&pt);
	ScreenToWindow(pt);
}

LRESULT CThemeWindow::OnNcMouseMove(WPARAM wParam, POINT pt)
{
	PerformPaint();

	if (!m_fTracking)
	{
		TRACKMOUSEEVENT tme;

		memset(&tme, 0, sizeof(tme));

		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_NONCLIENT | TME_LEAVE;
		tme.hwndTrack = GetHandle();

		TrackMouseEvent(&tme);
		
		m_fTracking = TRUE;
	}

	return 0;
}

LRESULT CThemeWindow::OnNcActivate(BOOL fActive)
{
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);

	return TRUE;
}

LRESULT CThemeWindow::OnNcLButtonDown(WPARAM wParam, POINT pt)
{
	m_fCapturing = TRUE;
	m_nCapturingButton = wParam;

	PerformPaint();

	SetCapture(GetHandle());

	return 0;
}

LRESULT CThemeWindow::OnLButtonUp(WPARAM wParam, POINT pt)
{
	ReleaseCapture();

	m_fCapturing = FALSE;

	THEMING_CALCS tc;

	ConstructCalcs(tc, pt);

	if (tc.htMinimize.fOver && m_nCapturingButton == HTMINBUTTON)
	{
		ExecuteSystemMenuCommand(SC_MINIMIZE);
	}
	else if (tc.htMaximize.fOver && m_nCapturingButton == HTMAXBUTTON)
	{
		if (tc.fZoomed)
		{
			ExecuteSystemMenuCommand(SC_RESTORE);
		}
		else
		{
			ExecuteSystemMenuCommand(SC_MAXIMIZE);
		}
	}
	else if (tc.htClose.fOver && m_nCapturingButton == HTCLOSE)
	{
		ExecuteSystemMenuCommand(SC_CLOSE);
	}

	m_nCapturingButton = 0;

	return 0;
}

LRESULT CThemeWindow::OnNcMouseLeave()
{
	PerformPaint();

	m_fTracking = FALSE;

	return 0;
}

void CThemeWindow::GetDrawingArea(RECT & rt)
{
	SIZE szSize;

	GetWindowSize(szSize);

	if (IsZoomed())
	{
		INT nFrameWidth = GetSystemMetrics(SM_CXFRAME);

		rt.left = nFrameWidth;
		rt.top = nFrameWidth;
		rt.right = szSize.cx - nFrameWidth;
		rt.bottom = szSize.cy - nFrameWidth;
	}
	else
	{
		rt.left = 0;
		rt.top = 0;
		rt.right = szSize.cx;
		rt.bottom = szSize.cy;
	}
}

LRESULT CThemeWindow::OnSetText(LPCWSTR szText)
{
	LRESULT lResult;

	// (( scope ))
	{
		CScopedRedrawLock lock(this);

		lResult = DefWindowProc(GetHandle(), WM_SETTEXT, 0, (LPARAM)szText);
	}

	PerformPaint();

	return lResult;
}

void CThemeWindow::PerformPaint()
{
	if (IsIconic() || GetUpdatesLocked())
	{
		return;
	}

	THEMING_CALCS tc;

	ConstructCalcs(tc);

	CWindowDC dc(this);

	dc.ExcludeClipRect(&tc.rcClientArea);

	CDoubleBufferedDC dcBuffered(this, dc);

	DrawFrame(dcBuffered, tc);

	dcBuffered.Complete();
}

LRESULT CThemeWindow::OnSetIcon(UINT nSizeType, HICON hIcon)
{
	LRESULT lResult;

	// (( scope ))
	{
		CScopedRedrawLock lock(this);

		lResult = DefWindowProc(GetHandle(), WM_SETICON, nSizeType, (LPARAM)hIcon);
	}

	PerformPaint();

	return lResult;
}
