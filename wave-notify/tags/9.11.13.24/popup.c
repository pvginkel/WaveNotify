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

#include "stdafx.h"
#include "include.h"

#define POPUP_CLASS L"PopupFrame"
#define IDLE_TIME 10000
#define ID_POPUP_TIMER 1
#define ID_MOUSEOVER_TIMER 2
#define ANIMATION_STEPS 20

typedef enum tagPOPUP_STATE
{
	STATE_PENDING,
	STATE_OPENING,
	STATE_WAITING,
	STATE_CLOSING
} POPUP_STATE;

static void Popup_ShowForm();
static void Popup_CreateForm();
static void Popup_RegisterClass();
static LRESULT CALLBACK Popup_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void Popup_GetAnimationBounds(LPRECT lpResult, LPPOPUP lpPopup);
static BOOL Popup_ShouldShow();
static void Popup_WmPaint(HWND hWnd);
static void Popup_Timer(HWND hWnd);
static void Popup_AnimatePending(HWND hWnd);
static void Popup_AnimateOpening(HWND hWnd);
static void Popup_AnimateWaiting(HWND hWnd);
static void Popup_AnimateClosing(HWND hWnd);
static void Popup_UpdateFromAnimationStep(HWND hWnd);
static void Popup_WmMouseMove(HWND hWnd);
static void Popup_WmMouseLeave(HWND hWnd);
static void Popup_ClearList();
static void Popup_MouseOverTimer(HWND hWnd);
static void Popup_DestroyPopup(LPPOPUP lpPopup);
static void Popup_CalculatePopupRect(LPPOPUP lpPopup);
static void Popup_OpeningComplete(HWND hWnd);
static void Popup_CompleteClosing(HWND hWnd);

static HWND g_hPopupForm = NULL;
static LPLIST g_lpPopupList = NULL;
static LPPOPUP g_lpCurrentPopup = NULL;
static ATOM g_uClass = 0;
static POPUP_STATE g_nState;
static INT g_nAnimationStep = 0;
static BOOL g_fMouseOver = FALSE;
static HCURSOR g_hCursorArrow = NULL;
static HCURSOR g_hCursorHand = NULL;

LPPOPUP Popup_Create()
{
	LPPOPUP lpPopup;

	lpPopup = ALLOCA(POPUP);

	memset(lpPopup, 0, sizeof(POPUP));

	lpPopup->nDuration = 5000;
	lpPopup->nHeight = 120;
	lpPopup->nWidth = 160;
	lpPopup->nAnimationDuration = 150;

	return lpPopup;
}

void Popup_Show(LPPOPUP lpPopup)
{
	if (g_lpPopupList == NULL)
	{
		g_lpPopupList = List_Create();
	}

	List_Add(g_lpPopupList, lpPopup);

	if (g_hPopupForm == NULL)
	{
		Popup_CreateForm();
	}
}

static void Popup_CreateForm()
{
	if (g_uClass == 0)
	{
		Popup_RegisterClass();
	}

	if (g_lpCurrentPopup != NULL)
	{
		Popup_DestroyPopup(g_lpCurrentPopup);
	}

	g_lpCurrentPopup = (LPPOPUP)List_Dequeue(g_lpPopupList);

	ASSERT(g_lpCurrentPopup != NULL);

	Popup_CalculatePopupRect(g_lpCurrentPopup);

	g_hPopupForm = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
		(LPWSTR)g_uClass,
		L"",
		WS_POPUP | WS_CLIPSIBLINGS,
		g_lpCurrentPopup->rLocation.left,
		g_lpCurrentPopup->rLocation.top,
		g_lpCurrentPopup->nWidth,
		g_lpCurrentPopup->nHeight,
		NULL,
		NULL,
		g_hInstance,
		NULL);

	if (Popup_ShouldShow())
	{
		Popup_ShowForm();
	}
	else
	{
		g_nState = STATE_PENDING;
		
		SetTimer(g_hPopupForm, ID_POPUP_TIMER, 200, NULL);
	}
}

static void Popup_RegisterClass()
{
	WNDCLASSEX wcex;

	if (g_hCursorArrow == NULL)
	{
		g_hCursorArrow = LoadCursor(NULL, IDC_ARROW);
		g_hCursorHand = LoadCursor(NULL, IDC_HAND);
	}

	memset(&wcex, 0, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Popup_WndProc;
	wcex.hInstance = g_hInstance;
	wcex.lpszClassName = POPUP_CLASS;

	g_uClass = RegisterClassEx(&wcex);
}

static LRESULT CALLBACK Popup_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		g_fMouseOver = FALSE;
		return 0;

	case WM_PAINT:
		Popup_WmPaint(hWnd);
		return 0;

	case WM_LBUTTONUP:
		if (g_lpCurrentPopup->lpClickedProc != NULL)
		{
			if (List_GetCount(g_lpPopupList) == 0)
			{
				ShowWindow(hWnd, SW_HIDE);
			}

			g_lpCurrentPopup->lpClickedProc(g_lpCurrentPopup);

			if (List_GetCount(g_lpPopupList) == 0)
			{
				DestroyWindow(hWnd);
			}
			else
			{
				g_nState = STATE_WAITING;
				Popup_AnimateWaiting(hWnd);
			}
		}
		return 0;

	case WM_RBUTTONUP:
		Popup_ClearList();
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		g_hPopupForm = NULL;

		if (g_lpCurrentPopup != NULL)
		{
			Popup_DestroyPopup(g_lpCurrentPopup);
			g_lpCurrentPopup = NULL;
		}

		// It could be a popup was queued while closing; start the new form

		if (List_GetCount(g_lpPopupList) > 0)
		{
			Popup_CreateForm();
		}
		return 0;

	case WM_TIMER:
		switch (wParam)
		{
		case ID_POPUP_TIMER:
			Popup_Timer(hWnd);
			break;

		case ID_MOUSEOVER_TIMER:
			Popup_MouseOverTimer(hWnd);
			break;
		}
		return 0;

	case WM_MOUSEMOVE:
		Popup_WmMouseMove(hWnd);
		return 0;

	case WM_MOUSELEAVE:
		Popup_WmMouseLeave(hWnd);
		return 0;

	case WM_SETCURSOR:
		SetCursor(g_lpCurrentPopup->lpClickedProc == NULL ? g_hCursorArrow : g_hCursorHand);
		return 1;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

static void Popup_MouseOverTimer(HWND hWnd)
{
	POINT p;
	RECT r;

	GetCursorPos(&p);
	GetWindowRect(hWnd, &r);

	if (!PtInRect(&r, p))
	{
		KillTimer(hWnd, ID_MOUSEOVER_TIMER);
		SendMessage(hWnd, WM_MOUSELEAVE, 0, 0);
	}
}

static void Popup_WmMouseMove(HWND hWnd)
{
	if (!g_fMouseOver)
	{
		SetTimer(hWnd, ID_MOUSEOVER_TIMER, 200, NULL);

		g_fMouseOver = TRUE;

		if (g_nState == STATE_WAITING)
		{
			KillTimer(hWnd, ID_POPUP_TIMER);
		}
	}
}

static void Popup_WmMouseLeave(HWND hWnd)
{
	if (g_fMouseOver)
	{
		g_fMouseOver = FALSE;

		if (g_nState == STATE_WAITING)
		{
			SetTimer(hWnd, ID_POPUP_TIMER, 2000, NULL);
		}
	}
}

static void Popup_WmPaint(HWND hWnd)
{
	PAINTSTRUCT ps;

	BeginPaint(hWnd, &ps);

	if (g_lpCurrentPopup->lpPaintProc != NULL)
	{
		g_lpCurrentPopup->lpPaintProc(hWnd, &ps, g_lpCurrentPopup);
	}

	EndPaint(hWnd, &ps);
}

static void Popup_Timer(HWND hWnd)
{
	switch (g_nState)
	{
	case STATE_PENDING: Popup_AnimatePending(hWnd); break;
	case STATE_OPENING: Popup_AnimateOpening(hWnd); break;
	case STATE_WAITING: Popup_AnimateWaiting(hWnd); break;
	case STATE_CLOSING: Popup_AnimateClosing(hWnd); break;
	}
}

static void Popup_AnimatePending(HWND hWnd)
{
	if (Popup_ShouldShow())
	{
		Popup_ShowForm();
	}
}

static void Popup_AnimateOpening(HWND hWnd)
{
	if (g_nAnimationStep == ANIMATION_STEPS)
	{
		Popup_OpeningComplete(hWnd);
	}
	else
	{
		Popup_UpdateFromAnimationStep(hWnd);

		g_nAnimationStep++;
	}
}

static void Popup_OpeningComplete(HWND hWnd)
{
	g_nAnimationStep = ANIMATION_STEPS;
	g_nState = STATE_WAITING;

	Popup_UpdateFromAnimationStep(hWnd);

	KillTimer(hWnd, ID_POPUP_TIMER);

	if (!g_fMouseOver)
	{
		SetTimer(hWnd, ID_POPUP_TIMER, g_lpCurrentPopup->nDuration, NULL);
	}
}

static void Popup_AnimateWaiting(HWND hWnd)
{
	KillTimer(hWnd, ID_POPUP_TIMER);

	if (List_GetCount(g_lpPopupList) > 0)
	{
		g_nState = STATE_WAITING;

		if (g_lpCurrentPopup != NULL)
		{
			Popup_DestroyPopup(g_lpCurrentPopup);
			g_lpCurrentPopup = NULL;
		}

		g_lpCurrentPopup = (LPPOPUP)List_Dequeue(g_lpPopupList);

		Popup_CalculatePopupRect(g_lpCurrentPopup);
		Popup_UpdateFromAnimationStep(hWnd);

		SetTimer(hWnd, ID_POPUP_TIMER, g_lpCurrentPopup->nDuration, NULL);

		InvalidateRect(hWnd, NULL, TRUE);
	}
	else if (DegradeVisualPerformance())
	{
		Popup_CompleteClosing(hWnd);
	}
	else
	{
		g_nState = STATE_CLOSING;
		g_nAnimationStep = ANIMATION_STEPS;

		SetTimer(hWnd, ID_POPUP_TIMER, g_lpCurrentPopup->nAnimationDuration / ANIMATION_STEPS, NULL);

		Popup_UpdateFromAnimationStep(hWnd);

		g_nAnimationStep--;
	}
}

static void Popup_CompleteClosing(HWND hWnd)
{
	g_nAnimationStep = 0;
	DestroyWindow(hWnd);
}

static void Popup_UpdateFromAnimationStep(HWND hWnd)
{
	INT nAnimationHeight = g_lpCurrentPopup->nHeight / 2;
	BYTE nOpacity;
	INT nOffset;
	RECT r;

	if (g_nAnimationStep == ANIMATION_STEPS)
	{
		nOpacity = 255;
		nOffset = 0;
	}
	else
	{
		nOpacity = (BYTE)((255 * g_nAnimationStep) / ANIMATION_STEPS);
		nOffset = nAnimationHeight - ((nAnimationHeight * g_nAnimationStep) / ANIMATION_STEPS);
	}

	r = g_lpCurrentPopup->rLocation;

	switch (g_lpCurrentPopup->uTaskbarEdge)
	{
	case TLE_LEFT:
		r.left -= nOffset;
		r.right -= nOffset;
		break;

	case TLE_RIGHT:
		r.left += nOffset;
		r.right += nOffset;
		break;

	case TLE_TOP:
		r.top -= nOffset;
		r.bottom -= nOffset;
		break;

	case TLE_BOTTOM:
		r.top += nOffset;
		r.bottom += nOffset;
		break;
	}

	SetLayeredWindowAttributes(hWnd, 0, nOpacity, LWA_ALPHA);

	SetWindowPos(
		hWnd,
		NULL,
		r.left,
		r.top,
		r.right - r.left,
		r.bottom - r.top,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

static void Popup_AnimateClosing(HWND hWnd)
{
	if (g_nAnimationStep == 0)
	{
		Popup_CompleteClosing(hWnd);
	}
	else
	{
		Popup_UpdateFromAnimationStep(hWnd);

		g_nAnimationStep--;
	}
}

static BOOL Popup_ShouldShow()
{
	RECT rForeground;
	LASTINPUTINFO lii;
	HMONITOR hMonitor;
	POINT p;
	MONITORINFOEX miex;

	memset(&lii, 0, sizeof(LASTINPUTINFO));

	lii.cbSize = sizeof(LASTINPUTINFO);

	GetLastInputInfo(&lii);

	if (GetTickCount() - lii.dwTime > IDLE_TIME)
	{
		return FALSE;
	}

	GetClientRect(GetForegroundWindow(), &rForeground);

	p.x = rForeground.left;
	p.y = rForeground.top;

	hMonitor = MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY);

	memset(&miex, 0, sizeof(MONITORINFOEX));

	miex.cbSize = sizeof(MONITORINFOEX);

	GetMonitorInfo(hMonitor, (LPMONITORINFO)&miex);

	return !(
		miex.rcMonitor.left == rForeground.left &&
		miex.rcMonitor.top == rForeground.top &&
		miex.rcMonitor.right == rForeground.right &&
		miex.rcMonitor.bottom == rForeground.bottom
	);
}

static void Popup_ShowForm()
{
	if (DegradeVisualPerformance())
	{
		Popup_OpeningComplete(g_hPopupForm);
	}
	else
	{
		g_nState = STATE_OPENING;
		g_nAnimationStep = 0;

		KillTimer(g_hPopupForm, ID_POPUP_TIMER);
		SetTimer(g_hPopupForm, ID_POPUP_TIMER, g_lpCurrentPopup->nAnimationDuration / ANIMATION_STEPS, NULL);

		SetLayeredWindowAttributes(g_hPopupForm, 0, 0, LWA_ALPHA);
	}

	ShowWindow(g_hPopupForm, SW_SHOWNOACTIVATE);
}

static void Popup_ClearList()
{
	LPPOPUP lpDelete;

	while (lpDelete = (LPPOPUP)List_Dequeue(g_lpPopupList))
	{
		if (lpDelete->lpClosedProc != NULL)
		{
			lpDelete->lpClosedProc(lpDelete);
		}

		FREE(lpDelete);
	}
}

void Popup_Destroy()
{
	LPPOPUP lpDelete;

	if (g_lpCurrentPopup != NULL)
	{
		Popup_DestroyPopup(g_lpCurrentPopup);
		g_lpCurrentPopup = NULL;
	}

	if (g_lpPopupList != NULL)
	{
		while (lpDelete = (LPPOPUP)List_Dequeue(g_lpPopupList))
		{
			Popup_DestroyPopup(lpDelete);
		}

		List_Destroy(g_lpPopupList);
	}
}

static void Popup_DestroyPopup(LPPOPUP lpPopup)
{
	if (lpPopup->lpClosedProc != NULL)
	{
		lpPopup->lpClosedProc(lpPopup);
	}

	FREE(lpPopup);
}

void Popup_CancelAll()
{
	if (g_hPopupForm != NULL)
	{
		Popup_ClearList();
		DestroyWindow(g_hPopupForm);
	}
}

static void Popup_CalculatePopupRect(LPPOPUP lpPopup)
{
	RECT r;
	SIZE sPopupSize;
	SIZE sOffset;
	TASKBARLOCATION tl;

	sPopupSize.cx = lpPopup->nWidth;
	sPopupSize.cy = lpPopup->nHeight;

	sOffset.cx = 0;
	sOffset.cy = 0;
	
	GetTaskbarPopupLocation(sPopupSize, sOffset, &r);

	lpPopup->rLocation = r;

	GetTaskbarLocation(&tl);

	lpPopup->uTaskbarEdge = tl.uEdge;
}
