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

#define APP_CLASS 		L"GoogleWaveNotifier"
#define APP_TITLE 		APP_CLASS

#define ID_NOTIFYICON 		1

#define ID_QUERY_TIMER		1
#define ID_VERSION_TIMER	2

#define QUERY_INTERVAL			(2 * 60 * 1000)		// 2 minutes
#define VERSION_INITIAL_INTERVAL	(10 * 60 * 1000)	// 10 minutes
#define VERSION_INTERVAL		(60 * 60 * 1000)	// 1 hour

//
// Popup layout information
//

#define PL_BORDER_WIDTH		2	// Outer border width
#define PL_WIDTH		264	// Width of all popups
#define PL_WAVE_HEIGHT		109	// Height of the Wave popup
#define PL_MSG_HEIGHT		65	// Height of the message popup
#define PL_PADDING		6	// Inner padding for popups within the border
#define PL_LINE_SPACING		3	// Spacing between different text blocks

// For 32x32 icons
//#define PL_ICON_SIZE		32	// Size the MainIcon is loaded with
//#define PL_TEXT_OFFSET	44	// Left offset of the text counting from padding
//#define PL_ICON_DX		0	// Full X adjustment of the icon
//#define PL_ICON_DY		0	// Full Y adjustment of the icon

// For 48x48 icons
#define PL_ICON_SIZE		48	// Size the MainIcon is loaded with
#define PL_TEXT_OFFSET		54	// Left offset of the text counting from padding
#define PL_ICON_DX		0	// Full X adjustment of the icon
#define PL_ICON_DY		-12	// Full Y adjustment of the icon

//
// Flyout layout information
//

#define FL_WIDTH		240	// Flyout width
#define FL_HEIGHT		300	// Flyout height
#define FL_BOTTOM_HEIGHT	30	// Height of the bottom panel
#define FL_SPACING		6	// Spacing between the different waves
#define FL_MAXIMUM_WAVES	5	// Maximum number of waves to show

// Use this to log all waves which triggered a popup to file.
// #define DUMP_WAVES

typedef enum
{
	FHT_NONE,
	FHT_WAVE,
	FHT_INBOX
} FLYOUTHITTEST;

typedef struct tagUNREAD_WAVE
{
	LPWSTR szId;
	LPWSTR szContact;
	LPWSTR szSubject;
	LPWSTR szBody;
	INT nIndex;
	INT nCount;
	INT nUnread;
	INT nHeight;
} UNREAD_WAVE, * LPUNREAD_WAVE;

static BOOL App_RegisterClass();
static BOOL App_CreateWindow();
static LRESULT CALLBACK App_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL App_Login(HWND hWnd);
static LPWAVE_SESSION App_CreateWaveSessionFromRegistry();
static void App_Logoff(HWND hWnd);
static void App_PromptCredentials(HWND hWnd);
static INT App_Query(HWND hWnd);
static void App_WmNotifyIcon(HWND hWnd, UINT message, UINT uID);
static void App_WmCommand(HWND hWnd, WORD wID);
static void App_ShowContextMenu(HWND hWnd);
static void App_CheckForUpdates(HWND hWnd);
static void App_ClearCredentials();
static void App_VersionCheck(HWND hWnd);
static void App_PopupPaintBackground(HWND hWnd, LPPAINTSTRUCT lpps, LPPOPUP lpPopup);
static void CALLBACK App_PopupPaintMessage(HWND hWnd, LPPAINTSTRUCT lpps, LPPOPUP lpPopup);
static void CALLBACK App_PopupNewVersionClicked(LPPOPUP lpPopup);
static LPWAVE_RESPONSE App_GetWaveResponse(BOOL fRetry);
static LPWAVE App_GetLastWave(LPWAVE lpWave);
static LPUNREAD_WAVE App_CreateUnreadWave(LPWAVE_RESPONSE lpResponse, LPWAVE lpWave, LPWAVE lpLastWave);
static void App_ProcessUnreadWavesNotifyIcon(INT nUnreadWaves);
static void App_ProcessUnreadWaves(LPLIST lpUnreadWaves);
static void CALLBACK App_PopupPaintUnreadWave(HWND hWnd, LPPAINTSTRUCT lpps, LPPOPUP lpPopup);
static void CALLBACK App_PopupUnreadWaveClicked(LPPOPUP lpPopup);
static void CALLBACK App_PopupUnreadWaveClosed(LPPOPUP lpPopup);
static LPUNREAD_WAVE App_CreateUnreadWaveFromWave(LPWAVE_RESPONSE lpResponse, LPWAVE lpWave, LPWAVE_MSG lpMessage);
static void App_ForcedQuery(HWND hWnd);
static void App_ShowFlyout(HWND hWnd);
static INT App_PaintUnreadWave(HDC hdc, LPRECT lpr, LPUNREAD_WAVE lpUnread, POINT p);
static LPLIST App_GetUnreadWaves();
static LRESULT CALLBACK App_FlyoutWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void App_FlyoutWmPaint(HWND hWnd);
static INT App_FlyoutPaintBottom(HDC hdc, LPRECT lpr, POINT p, LPLIST lpUnread, BOOL fInvalidate);
static INT App_CalculateFlyoutHeight(LPLIST lpUnread);
static void App_FlyoutWmMouseMove(HWND hWnd, WORD x, WORD y);
static HFONT App_GetUrlFont();
static void App_FlyoutWmLButtonUp(HWND hWnd);
static void App_FlyoutPaint(HWND hWnd, HDC hdc, POINT p, BOOL fInvalidate);
static void App_DestroyUnreadWave(LPUNREAD_WAVE lpUnread);
static void App_DestroyUnreadWaves(LPLIST lpUnread);
static void App_FlyoutPaintWaves(HWND hWnd, HDC hdc, POINT p, LPRECT lpr, LPLIST lpUnread, BOOL fInvalidate);
static void App_FlyoutPaintNoWaves(HWND hWnd, HDC hdc, LPRECT lpr);

#if defined(_DEBUG) && defined(DUMP_WAVES)
static void App_LogWaveDifference(LPWSTR szMessage, INT nMessage, LPWAVE lpLastWave, LPWAVE lpWave);
#endif

LPWAVE_SESSION g_lpWaveSession = NULL;

static ATOM g_uClass = 0;
static HWND g_hWnd = NULL;
static LPNOTIFYICON g_lpNotifyIcon = NULL;
static HICON g_hNotifyIcon = NULL;
static HICON g_hNotifyIconGray = NULL;
static HICON g_hNotifyIconUnread = NULL;
static HMENU g_hPopupMenus = NULL;
static LPWAVE_RESPONSE g_lpLastWaveResponse = NULL;
static FLYOUTHITTEST g_nFlyoutHitTest = FHT_NONE;
static HCURSOR g_hCursorArrow = NULL;
static HCURSOR g_hCursorHand = NULL;
static LPUNREAD_WAVE g_lpHoveringWave = NULL;
static UINT g_uWmTaskbarCreated;

BOOL App_Initialise()
{
	g_hNotifyIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_TRAYICON));
	g_hNotifyIconGray = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_TRAYICON_GRAY));
	g_hNotifyIconUnread = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_TRAYICON_UNREAD));

	g_uWmTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");

	if (!App_RegisterClass())
	{
		return FALSE;
	}

	if (!App_CreateWindow())
	{
		return FALSE;
	}

	if (g_hCursorArrow == NULL)
	{
		g_hCursorArrow = LoadCursor(NULL, IDC_ARROW);
		g_hCursorHand = LoadCursor(NULL, IDC_HAND);
	}

	return TRUE;
}

void App_Destroy()
{
	if (g_hWnd != NULL)
	{
		DestroyWindow(g_hWnd);
	}

	if (g_lpWaveSession != NULL)
	{
		Wave_DestroySession(g_lpWaveSession);
		g_lpWaveSession = NULL;
	}

	if (g_lpLastWaveResponse != NULL)
	{
		Wave_DestroyResponse(g_lpLastWaveResponse);
		g_lpLastWaveResponse = NULL;
	}

	Popup_Destroy();
}

static BOOL App_RegisterClass()
{
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.lpfnWndProc = App_WndProc;
	wcex.hInstance = g_hInstance;
	wcex.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = APP_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MAINICON_SMALL));

	g_uClass = RegisterClassEx(&wcex);

	return g_uClass != 0;
}

static BOOL App_CreateWindow()
{
   	g_hWnd = CreateWindow(
		(LPWSTR)g_uClass,
		APP_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		g_hInstance,
		NULL);

	return g_hWnd != NULL;
}

static LRESULT CALLBACK App_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		if (!App_Login(hWnd))
		{
			DestroyWindow(hWnd);
		}

		SetTimer(hWnd, ID_VERSION_TIMER, VERSION_INITIAL_INTERVAL, NULL);
		return 0;

	case WM_DESTROY:
		App_Logoff(hWnd);

		PostQuitMessage(0);
		return 0;

	case WM_NOTIFTICON:
		App_WmNotifyIcon(hWnd, (UINT)lParam, (UINT)wParam);
		return 0;

	case WM_COMMAND:
		App_WmCommand(hWnd, LOWORD(wParam));
		return 0;

	case WM_TIMER:
		switch (wParam)
		{
		case ID_QUERY_TIMER:
			App_Query(hWnd);
			return 0;

		case ID_VERSION_TIMER:
			App_VersionCheck(hWnd);
			return 0;
		}
		break;

	default:
		if (message == g_uWmTaskbarCreated && g_lpNotifyIcon != NULL)
		{
			NotifyIcon_Recreate(g_lpNotifyIcon);
		}
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

static BOOL App_Login(HWND hWnd)
{
	if (g_lpWaveSession == NULL)
	{
		g_lpWaveSession = App_CreateWaveSessionFromRegistry();
	}

	if (g_lpWaveSession != NULL && !Wave_IsLoggedOn(g_lpWaveSession))
	{
		Wave_Login(g_lpWaveSession);
	}

	if (g_lpWaveSession == NULL || !Wave_IsLoggedOn(g_lpWaveSession))
	{
		if (g_lpWaveSession != NULL)
		{
			Wave_DestroySession(g_lpWaveSession);
			g_lpWaveSession = NULL;
		}

		App_PromptCredentials(hWnd);
	}
	else
	{
		if (g_lpNotifyIcon == NULL)
		{
			g_lpNotifyIcon = NotifyIcon_Create(hWnd, ID_NOTIFYICON, g_hNotifyIconGray, L"Loading...");
		}
		else
		{
			NotifyIcon_SetIcon(g_lpNotifyIcon, g_hNotifyIconGray);
			NotifyIcon_SetTooltip(g_lpNotifyIcon, L"Loading...");
		}

		App_Query(hWnd);

		SetTimer(hWnd, ID_QUERY_TIMER, QUERY_INTERVAL, NULL);
	}

	return g_lpWaveSession != NULL && Wave_IsLoggedOn(g_lpWaveSession);
}

static void App_Logoff(HWND hWnd)
{
	if (g_lpWaveSession != NULL && Wave_IsLoggedOn(g_lpWaveSession))
	{
		Wave_Logoff(g_lpWaveSession);
	}

	if (g_lpLastWaveResponse != NULL)
	{
		Wave_DestroyResponse(g_lpLastWaveResponse);
		g_lpLastWaveResponse = NULL;
	}

	KillTimer(hWnd, ID_QUERY_TIMER);

	if (g_lpNotifyIcon != NULL)
	{
		NotifyIcon_Destroy(g_lpNotifyIcon);
		g_lpNotifyIcon = NULL;
	}
}

static void App_PromptCredentials(HWND hWnd)
{
	LoginForm_ShowDialog(hWnd);

	if (g_lpWaveSession == NULL || !Wave_IsLoggedOn(g_lpWaveSession))
	{
		SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
	else
	{
		App_Login(hWnd);
	}
}

static LPWAVE_SESSION App_CreateWaveSessionFromRegistry()
{
	HKEY hBaseKey = NULL;
	LPWSTR szUsername = NULL;
	LPWSTR szPassword = NULL;
	WCHAR szBuffer[REGVALUE_SIZE];
	DWORD dwType;
	DWORD dwSize;
	LPWAVE_SESSION lpSession = NULL;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_BASEKEY, 0, KEY_READ, &hBaseKey) != ERROR_SUCCESS)
	{
		goto __end;
	}

	dwSize = _ARRAYSIZE(szBuffer);

	if (
		RegQueryValueEx(hBaseKey, REG_USERNAME, NULL, &dwType, (LPBYTE)szBuffer, &dwSize) != ERROR_SUCCESS ||
		dwType != REG_SZ
	) {
		goto __end;
	}

	szUsername = _wcsdup(szBuffer);

	dwSize = _ARRAYSIZE(szBuffer);

	if (
		RegQueryValueEx(hBaseKey, REG_PASSWORD, NULL, &dwType, (LPBYTE)szBuffer, &dwSize) != ERROR_SUCCESS ||
		dwType != REG_SZ
	) {
		goto __end;
	}

	szPassword = _wcsdup(szBuffer);

	lpSession = Wave_CreateSession(szUsername, szPassword);

__end:
	if (hBaseKey != NULL)
	{
		RegCloseKey(hBaseKey);
	}
	if (szUsername != NULL)
	{
		FREE(szUsername);
	}
	if (szPassword != NULL)
	{
		FREE(szPassword);
	}

	return lpSession;
}

static void App_WmNotifyIcon(HWND hWnd, UINT message, UINT uID)
{
	switch (message)
	{
	case WM_LBUTTONUP:
		App_ShowFlyout(hWnd);
		break;

	case WM_RBUTTONUP:
		App_ShowContextMenu(hWnd);
		break;
	}
}

static void App_ShowContextMenu(HWND hWnd)
{
	HMENU hSubMenu;
	POINT p;

	if (g_hPopupMenus != NULL)
	{
		DestroyMenu(g_hPopupMenus);
	}

	g_hPopupMenus = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_POPUP_MENUS));

	hSubMenu = GetSubMenu(g_hPopupMenus, 0);

	GetCursorPos(&p);

	SetForegroundWindow(hWnd);

	TrackPopupMenuEx(
		hSubMenu,
		TPM_VERTICAL | TPM_RIGHTALIGN,
		p.x,
		p.y,
		hWnd,
		NULL);

	PostMessage(hWnd, WM_NULL, 0, 0);
}

static void App_WmCommand(HWND hWnd, WORD wID)
{
	switch (wID)
	{
	case ID_TRAYICON_EXIT:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case ID_TRAYICON_CHECKWAVESNOW:
		App_ForcedQuery(hWnd);
		break;

	case ID_TRAYICON_CHECKFORUPDATESNOW:
		App_CheckForUpdates(hWnd);
		break;

	case ID_TRAYICON_INBOX:
		Wave_OpenInbox(g_lpWaveSession);
		break;

	case ID_TRAYICON_SIGNOUT:
		App_ClearCredentials();
		App_Logoff(hWnd);
		App_PromptCredentials(hWnd);
		break;
	}
}

static void App_CheckForUpdates(HWND hWnd)
{
	if (Version_IsNewAvailable())
	{
		NewVersionForm_ShowDialog(hWnd);
	}
	else
	{
		LPPOPUP lpPopup = Popup_Create();

		lpPopup->nHeight = PL_MSG_HEIGHT;
		lpPopup->nWidth = PL_WIDTH;
		lpPopup->nDuration = 3000;
		lpPopup->lpPaintProc = App_PopupPaintMessage;
		lpPopup->nTag = (INT_PTR)L"Google Wave Notifier is up to date";

		Popup_Show(lpPopup);
	}
}

static void App_ClearCredentials()
{
	HKEY hBaseKey;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_BASEKEY, 0, KEY_WRITE, &hBaseKey) == ERROR_SUCCESS)
	{
		RegDeleteValue(hBaseKey, REG_PASSWORD);

		RegCloseKey(hBaseKey);
	}
}

static void App_VersionCheck(HWND hWnd)
{
	KillTimer(hWnd, ID_VERSION_TIMER);
	SetTimer(hWnd, ID_VERSION_TIMER, VERSION_INTERVAL, NULL);

	if (Version_IsNewAvailable())
	{
		LPPOPUP lpPopup = Popup_Create();

		lpPopup->nHeight = PL_MSG_HEIGHT;
		lpPopup->nWidth = PL_WIDTH;
		lpPopup->nDuration = 3000;
		lpPopup->lpPaintProc = App_PopupPaintMessage;
		lpPopup->lpClickedProc = App_PopupNewVersionClicked;
		lpPopup->nTag = (INT_PTR)L"A new version of Google Wave Notifier is available";

		Popup_Show(lpPopup);
	}
}

static void CALLBACK App_PopupPaintMessage(HWND hWnd, LPPAINTSTRUCT lpps, LPPOPUP lpPopup)
{
	HDC hdc = lpps->hdc;
	LPWAVE lpWave = (LPWAVE)lpPopup->nTag;
	RECT rClient;
	RECT r;
	HGDIOBJ hOriginal;
	TEXTMETRIC tm;
	INT nHeight;
	INT nOffset;
	INT nMinOffset;
	
	App_PopupPaintBackground(hWnd, lpps, lpPopup);

	GetClientRect(hWnd, &rClient);
	InflateRect(&rClient, -(PL_BORDER_WIDTH + PL_PADDING), -(PL_BORDER_WIDTH + PL_PADDING));

	hOriginal = SelectFont(hdc, GetMessageBoxFont());

	memset(&tm, 0, sizeof(TEXTMETRIC));

	GetTextMetrics(hdc, &tm);

	r = rClient;
	r.left += PL_TEXT_OFFSET;

	DrawText(hdc, (LPWSTR)lpPopup->nTag, -1, &r, DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK);

	nHeight = ((r.bottom - r.top) / tm.tmHeight) * tm.tmHeight;
	nOffset = ((rClient.bottom - rClient.top) - nHeight) / 2;
	nMinOffset = ((PL_ICON_SIZE + PL_ICON_DY) - tm.tmHeight) / 2;

	if (nOffset > nMinOffset)
	{
		nOffset = nMinOffset;
	}

	r = rClient;
	r.left += PL_TEXT_OFFSET;
	r.top += nOffset;
	r.bottom = r.top + nHeight;

	DrawText(hdc, (LPWSTR)lpPopup->nTag, -1, &r, DT_NOPREFIX | DT_WORDBREAK);

	SelectObject(hdc, hOriginal);
}

static void App_PopupPaintBackground(HWND hWnd, LPPAINTSTRUCT lpps, LPPOPUP lpPopup)
{
	HDC hdc = lpps->hdc;
	RECT rClient;
	INT i;
	
	static BOOL fInitialised = FALSE;
	static HBRUSH hBorderBrush;
	static HICON hIcon;
	static HBRUSH hBackgroundBrush;
	static HBITMAP hBitmap;

	if (!fInitialised)
	{
		fInitialised = TRUE;
		hBorderBrush = CreateSolidBrush(RGB(237, 135, 0));
		hIcon = (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_MAINICON), IMAGE_ICON, PL_ICON_SIZE, PL_ICON_SIZE, 0);
		hBackgroundBrush = GetStockBrush(WHITE_BRUSH);
	}

	GetClientRect(hWnd, &rClient);

	for (i = 0; i < PL_BORDER_WIDTH; i++)
	{
		FrameRect(hdc, &rClient, hBorderBrush);
		InflateRect(&rClient, -1, -1);
	}

	FillRect(hdc, &rClient, hBackgroundBrush);

	DrawIconEx(
		hdc,
		(PL_BORDER_WIDTH + PL_PADDING) + (PL_ICON_DX / 2),
		(PL_BORDER_WIDTH + PL_PADDING) + (PL_ICON_DY / 2),
		hIcon,
		PL_ICON_SIZE,
		PL_ICON_SIZE,
		0,
		NULL,
		DI_NORMAL);
}

static void CALLBACK App_PopupNewVersionClicked(LPPOPUP lpPopup)
{
	NewVersionForm_ShowDialog(g_hWnd);
}

static INT App_Query(HWND hWnd)
{
	LPWAVE_RESPONSE lpResponse;
	LPLIST lpUnreadWaves;
	INT nUnreadWaves = 0;
	INT i;
	LPWAVE lpLastWave;
	LPWAVE lpWave;
	LPUNREAD_WAVE lpUnreadWave;

	KillTimer(hWnd, ID_QUERY_TIMER);

	lpResponse = App_GetWaveResponse(FALSE);

	if (lpResponse == NULL)
	{
		nUnreadWaves = -1;
		goto __end;
	}

	lpUnreadWaves = List_Create();

	for (i = 0; i < lpResponse->nWavesCount; i++)
	{
		lpWave = lpResponse->lpWaves[i];

		if (lpWave->nUnreadMessages > 0)
		{
			nUnreadWaves++;

			lpLastWave = App_GetLastWave(lpWave);

			lpUnreadWave = App_CreateUnreadWave(lpResponse, lpWave, lpLastWave);

			if (lpUnreadWave != NULL)
			{
				List_Add(lpUnreadWaves, lpUnreadWave);
			}
		}
	}

	if (g_lpLastWaveResponse != NULL)
	{
		Wave_DestroyResponse(g_lpLastWaveResponse);
	}

	g_lpLastWaveResponse = lpResponse;

	App_ProcessUnreadWavesNotifyIcon(nUnreadWaves);
	App_ProcessUnreadWaves(lpUnreadWaves);

__end:
	SetTimer(hWnd, ID_QUERY_TIMER, QUERY_INTERVAL, NULL);

	return nUnreadWaves;
}

static void App_ProcessUnreadWavesNotifyIcon(INT nUnreadWaves)
{
	WCHAR szBuffer[120];

	switch (nUnreadWaves)
	{
	case 0:
		NotifyIcon_SetIcon(g_lpNotifyIcon, g_hNotifyIcon);
		NotifyIcon_SetTooltip(g_lpNotifyIcon, L"No unread waves");
		break;

	case 1:
		NotifyIcon_SetIcon(g_lpNotifyIcon, g_hNotifyIconUnread);
		NotifyIcon_SetTooltip(g_lpNotifyIcon, L"1 unread wave");
		break;

	default:
		StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"%d unread waves", nUnreadWaves);
		NotifyIcon_SetIcon(g_lpNotifyIcon, g_hNotifyIconUnread);
		NotifyIcon_SetTooltip(g_lpNotifyIcon, szBuffer);
		break;
	}
}

static void App_ProcessUnreadWaves(LPLIST lpUnreadWaves)
{
	LPUNREAD_WAVE lpUnread;
	INT nIndex;
	INT nCount;

	nIndex = 1;
	nCount = List_GetCount(lpUnreadWaves);

	while (lpUnread = (LPUNREAD_WAVE)List_Dequeue(lpUnreadWaves))
	{
		LPPOPUP lpPopup;

		lpUnread->nIndex = nIndex++;
		lpUnread->nCount = nCount;

		lpPopup = Popup_Create();

		lpPopup->nWidth = PL_WIDTH;
		lpPopup->nHeight = PL_WAVE_HEIGHT;
		lpPopup->nDuration = 3600;
		lpPopup->nTag = (INT_PTR)lpUnread;
		lpPopup->lpPaintProc = App_PopupPaintUnreadWave;
		lpPopup->lpClickedProc = App_PopupUnreadWaveClicked;
		lpPopup->lpClosedProc = App_PopupUnreadWaveClosed;

		Popup_Show(lpPopup);
	}

	List_Destroy(lpUnreadWaves);
}

static LPWAVE_RESPONSE App_GetWaveResponse(BOOL fRetry)
{
	LPWAVE_RESPONSE lpWaveResponse;

	// If we could not reconnect during the last query, reconnect.

	if (!Wave_IsLoggedOn(g_lpWaveSession))
	{
		NotifyIcon_SetIcon(g_lpNotifyIcon, g_hNotifyIconGray);
		NotifyIcon_SetTooltip(g_lpNotifyIcon, L"Reconnecting...");

		Wave_Login(g_lpWaveSession);
	}

	if (!Wave_IsLoggedOn(g_lpWaveSession))
	{
		return NULL;
	}

	lpWaveResponse = Wave_Query(g_lpWaveSession);

	if (lpWaveResponse != NULL || fRetry)
	{
		return lpWaveResponse;
	}

	// When there was a problem retrieving the waves, we expect a session timeout
	// or a network disconnect. Just log off and try to log on on the next query.

	Wave_Logoff(g_lpWaveSession);

	return App_GetWaveResponse(TRUE);
}

static LPWAVE App_GetLastWave(LPWAVE lpWave)
{
	INT i;

	if (g_lpLastWaveResponse == NULL)
	{
		return NULL;
	}
	
	for (i = 0; i < g_lpLastWaveResponse->nWavesCount; i++)
	{
		if (wcscmp(g_lpLastWaveResponse->lpWaves[i]->szId, lpWave->szId) == 0)
		{
			return g_lpLastWaveResponse->lpWaves[i];
		}
	}

	return NULL;
}

static LPUNREAD_WAVE App_CreateUnreadWave(LPWAVE_RESPONSE lpResponse, LPWAVE lpWave, LPWAVE lpLastWave)
{
	LPWAVE_MSG lpMessage = NULL;
	INT i;
	BOOL fUnread = FALSE;

	// If there is no previous, the wave is a new one and it is unread.

	if (lpLastWave == NULL)
	{
#if defined(_DEBUG) && defined(DUMP_WAVES)
		App_LogWaveDifference(L"No previous", -1, lpLastWave, lpWave);
#endif

		fUnread = TRUE;
	}

	// For some reason, waves are sometimes inconsistently reported. It seems that the
	// messages and unread count are relatively stable. Because of this, we first
	// test for the messages and unread count before checking the contents.
	//
	// When the reported waves become stable, the statement below can be changed to a
	// simple else.

	else if (lpLastWave->nMessages != lpWave->nMessages || lpLastWave->nUnreadMessages != lpWave->nUnreadMessages)
	{
		// Compare the waves by content of the messages; find the first message that
		// is different. When a message is inserted into the wave, the new wave becomes
		// longer and we detect that this way.

		lpMessage = NULL;

		for (i = 0; i < lpWave->nCount; i++)
		{
			// If the last wave had less messages, it is unread.

			if (lpLastWave->nCount < i + 1)
			{
#if defined(_DEBUG) && defined(DUMP_WAVES)
				App_LogWaveDifference(L"Last wave had less messages", i, lpLastWave, lpWave);
#endif

				lpMessage = lpWave->lpMessages[i];
				fUnread = TRUE;
				break;
			}

			// If the sender or text has changed, it is unread.

#if defined(_DEBUG) && defined(DUMP_WAVES)
			if (_wcsicmp(lpLastWave->lpMessages[i]->szText, lpWave->lpMessages[i]->szText) != 0)
			{
				App_LogWaveDifference(L"Tests differ", i, lpLastWave, lpWave);
			}
			else if (_wcsicmp(lpLastWave->lpMessages[i]->szEmailAddress, lpWave->lpMessages[i]->szEmailAddress) != 0)
			{
				App_LogWaveDifference(L"Senders differ", i, lpLastWave, lpWave);
			}
#endif

			if (
				_wcsicmp(lpLastWave->lpMessages[i]->szText, lpWave->lpMessages[i]->szText) != 0 ||
				_wcsicmp(lpLastWave->lpMessages[i]->szEmailAddress, lpWave->lpMessages[i]->szEmailAddress) != 0
			) {
				lpMessage = lpWave->lpMessages[i];
				fUnread = TRUE;
				break;
			}
		}

		// When the messages are the same, but the subject has changed; see
		// this as a change too.

		if (
			lpMessage == NULL &&
			_wcsicmp(lpLastWave->szSubject, lpWave->szSubject) != 0
		) {
#if defined(_DEBUG) && defined(DUMP_WAVES)
			App_LogWaveDifference(L"Subjects differ", -1, lpLastWave, lpWave);
#endif

			fUnread = TRUE;
		}

		// If it still wasn't detected as changed and and the last wave did
		// not have unread messages, set it to changed.

		if (!fUnread && lpLastWave->nUnreadMessages == 0)
		{
#if defined(_DEBUG) && defined(DUMP_WAVES)
			App_LogWaveDifference(L"Last wave did not have unread messages", -1, lpLastWave, lpWave);
#endif

			fUnread = TRUE;
		}
	}

	if (fUnread)
	{
		// If no correct changed message could be detected, we just send the last
		// message in the wave.

		if (lpMessage == NULL && lpWave->nCount > 0)
		{
			lpMessage = lpWave->lpMessages[lpWave->nCount - 1];
		}

		return App_CreateUnreadWaveFromWave(lpResponse, lpWave, lpMessage);
	}

	return NULL;
}

static LPWSTR App_GetContact(LPWAVE_RESPONSE lpResponse, LPWSTR szEmailAddress)
{
	INT i;

	if (
		g_lpWaveSession->szEmailAddress != NULL &&
		_wcsicmp(g_lpWaveSession->szEmailAddress, szEmailAddress) == 0)
	{
		return L"Me";
	}

	for (i = 0; i < lpResponse->nContactsCount; i++)
	{
		if (_wcsicmp(lpResponse->lpContacts[i]->szEmailAddress, szEmailAddress) == 0)
		{
			return lpResponse->lpContacts[i]->szName;
		}
	}

	return szEmailAddress;
}

static LPUNREAD_WAVE App_CreateUnreadWaveFromWave(LPWAVE_RESPONSE lpResponse, LPWAVE lpWave, LPWAVE_MSG lpMessage)
{
	LPWSTR szEmailAddress;
	LPUNREAD_WAVE lpUnread;

	szEmailAddress =
		lpMessage == NULL || lpMessage->nOrder == 0 ?
		lpWave->szEmailAddress :
		lpMessage->szEmailAddress;

	lpUnread = ALLOCA(UNREAD_WAVE);

	memset(lpUnread, 0, sizeof(UNREAD_WAVE));

	lpUnread->szId = _wcsdup(lpWave->szId);
	lpUnread->szContact = _wcsdup(App_GetContact(lpResponse, szEmailAddress));
	lpUnread->szSubject = _wcsdup(lpWave->szSubject);
	lpUnread->nUnread = lpWave->nUnreadMessages;

	if (lpMessage != NULL)
	{
		lpUnread->szBody = _wcsdup(lpMessage->szText);
	}

	return lpUnread;
}

static void CALLBACK App_PopupPaintUnreadWave(HWND hWnd, LPPAINTSTRUCT lpps, LPPOPUP lpPopup)
{
	HDC hdc = lpps->hdc;
	RECT rClient;
	HGDIOBJ hOriginal;
	LPUNREAD_WAVE lpUnread = (LPUNREAD_WAVE)lpPopup->nTag;
	WCHAR szBuffer[200];
	RECT r;
	POINT p;

	static BOOL fInitialised = FALSE;
	static HFONT hFont;

	p.x = -1;
	p.y = -1;

	if (!fInitialised)
	{
		hFont = GetMessageBoxFont();
	}

	App_PopupPaintBackground(hWnd, lpps, lpPopup);

	GetClientRect(hWnd, &rClient);
	InflateRect(&rClient, -(PL_BORDER_WIDTH + PL_PADDING), -(PL_BORDER_WIDTH + PL_PADDING));

	r = rClient;
	r.left += PL_TEXT_OFFSET;

	App_PaintUnreadWave(hdc, &r, lpUnread, p);

	//
	// Paint the Index and Count
	//

	hOriginal = SelectObject(hdc, hFont);

	r = rClient;
	r.right = r.left + PL_ICON_SIZE + PL_ICON_DX;
	r.top += PL_ICON_SIZE + PL_ICON_DY + PL_LINE_SPACING;

	StringCbPrintf(szBuffer, _ARRAYSIZE(szBuffer), L"%d / %d", lpUnread->nIndex, lpUnread->nCount);

	DrawText(hdc, szBuffer, -1, &r, DT_CENTER | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	SelectObject(hdc, hOriginal);
}

static INT App_PaintUnreadWave(HDC hdc, LPRECT lpr, LPUNREAD_WAVE lpUnread, POINT p)
{
	RECT r;
	INT nOffset;
	WCHAR szBuffer[200];
	HGDIOBJ hOriginal;
	TEXTMETRIC tm;
	BOOL fCalculate = hdc == NULL;
	HFONT hSelectedFont;
	HFONT hSelectedBoldFont;
	INT nFullHeight;
	INT nMaxHeight;

	static BOOL fInitialised = FALSE;
	static HFONT hFont;
	static HFONT hBoldFont;
	static HFONT hUnderlineFont;
	static HFONT hUnderlineBoldFont;

	if (!fInitialised)
	{
		hFont = GetMessageBoxFont();
		hBoldFont = GetBoldFont(hFont);
		hUnderlineFont = App_GetUrlFont();
		hUnderlineBoldFont = GetBoldFont(hUnderlineFont);
	}

	if (fCalculate)
	{
		hdc = GetDC(NULL);
	}

	//
	// Paint the From
	//

	if (PtInRect(lpr, p))
	{
		SetTextColor(hdc, RGB(0, 0, 255));
		hSelectedFont = hUnderlineFont;
		hSelectedBoldFont = hUnderlineBoldFont;
	}
	else
	{
		SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
		hSelectedFont = hFont;
		hSelectedBoldFont = hBoldFont;
	}

	hOriginal = SelectFont(hdc, hSelectedBoldFont);

	r = *lpr;

	DrawText(hdc, lpUnread->szContact, -1, &r, DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	nOffset = r.bottom + PL_LINE_SPACING;

	if (!fCalculate)
	{
		DrawText(hdc, lpUnread->szContact, -1, &r, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);
	}

	//
	// Paint unread
	//

	if (!fCalculate && lpUnread->nUnread > 1)
	{
		StringCbPrintf(szBuffer, _ARRAYSIZE(szBuffer), L" (%d)", lpUnread->nUnread);

		r.left = r.right;
		r.right = lpr->right;

		// Do not paint when there is no space to paint

		if (r.left < r.right)
		{
			SelectFont(hdc, hSelectedFont);

			DrawText(hdc, szBuffer, -1, &r, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

			SelectFont(hdc, hSelectedBoldFont);
		}
	}

	//
	// Paint the Subject
	//

	r = *lpr;
	r.top = nOffset;

	DrawText(hdc, lpUnread->szSubject, -1, &r, DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);
	
	nOffset = r.bottom + PL_LINE_SPACING;

	if (!fCalculate)
	{
		DrawText(hdc, lpUnread->szSubject, -1, &r, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);
	}

	//
	// Paint the Body
	//

	SelectFont(hdc, hSelectedFont);

	memset(&tm, 0, sizeof(TEXTMETRIC));

	GetTextMetrics(hdc, &tm);

	r = *lpr;
	r.top = nOffset;

	DrawText(hdc, lpUnread->szBody, -1, &r, DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_WORDBREAK);

	nFullHeight = ((r.bottom - r.top) / tm.tmHeight) * tm.tmHeight;
	nMaxHeight = ((lpr->bottom - nOffset) / tm.tmHeight) * tm.tmHeight;

	r.bottom = r.top + min(nFullHeight, nMaxHeight);

	if (!fCalculate)
	{
		DrawText(hdc, lpUnread->szBody, -1, &r, DT_END_ELLIPSIS | DT_NOPREFIX | DT_WORDBREAK);
	}

	SelectObject(hdc, hOriginal);

	if (fCalculate)
	{
		DeleteDC(hdc);
	}

	return r.bottom - lpr->top;
}

static void CALLBACK App_PopupUnreadWaveClicked(LPPOPUP lpPopup)
{
	Wave_OpenWave(g_lpWaveSession, ((LPUNREAD_WAVE)lpPopup->nTag)->szId);
}

static void CALLBACK App_PopupUnreadWaveClosed(LPPOPUP lpPopup)
{
	App_DestroyUnreadWave((LPUNREAD_WAVE)lpPopup->nTag);
}

static void App_DestroyUnreadWave(LPUNREAD_WAVE lpUnread)
{
	if (lpUnread->szBody != NULL)
	{
		FREE(lpUnread->szBody);
	}
	if (lpUnread->szContact != NULL)
	{
		FREE(lpUnread->szContact);
	}
	if (lpUnread->szId != NULL)
	{
		FREE(lpUnread->szId);
	}
	if (lpUnread->szSubject != NULL)
	{
		FREE(lpUnread->szSubject);
	}
	
	FREE(lpUnread);
}

static void App_ForcedQuery(HWND hWnd)
{
	INT nUnreadWaves;

	if (g_lpLastWaveResponse != NULL)
	{
		Wave_DestroyResponse(g_lpLastWaveResponse);
		g_lpLastWaveResponse = NULL;
	}

	nUnreadWaves = App_Query(hWnd);

	if (nUnreadWaves == -1)
	{
		LPPOPUP lpPopup = Popup_Create();

		lpPopup->nHeight = PL_MSG_HEIGHT;
		lpPopup->nWidth = PL_WIDTH;
		lpPopup->nDuration = 3000;
		lpPopup->lpPaintProc = App_PopupPaintMessage;
		lpPopup->nTag = (INT_PTR)L"A problem occured while retrieving your Waves. Google Wave Notifier will reconnect in a moment.";

		Popup_Show(lpPopup);
	}
	else if (nUnreadWaves == 0)
	{
		LPPOPUP lpPopup = Popup_Create();

		lpPopup->nHeight = PL_MSG_HEIGHT;
		lpPopup->nWidth = PL_WIDTH;
		lpPopup->nDuration = 3000;
		lpPopup->lpPaintProc = App_PopupPaintMessage;
		lpPopup->nTag = (INT_PTR)L"No unread waves";

		Popup_Show(lpPopup);
	}
}

#if defined(_DEBUG) && defined(DUMP_WAVES)

static void App_DumpWave(HANDLE hFile, LPWAVE lpWave)
{
	WCHAR szBuffer[1024];
	DWORD dwWritten;
	INT i;

	wcscpy_s(szBuffer, _ARRAYSIZE(szBuffer), L"-------------------------------------------------------------------------------------\r\n");
	WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

	if (lpWave == NULL)
	{
		wcscpy_s(szBuffer, _ARRAYSIZE(szBuffer), L"(null)\r\n");
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);
	}
	else
	{
		StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"id: %s\r\n", lpWave->szId);
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"messages: %d\r\n", lpWave->nMessages);
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"unread messages: %d\r\n", lpWave->nUnreadMessages);
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"subject: %s\r\n", lpWave->szSubject);
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"message count: %d\r\n", lpWave->nCount);
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"e-mail: %s\r\n", lpWave->szEmailAddress);
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		for (i = 0; i < lpWave->nCount; i++)
		{
			LPWAVE_MSG lpMessage = lpWave->lpMessages[i];

			wcscpy_s(szBuffer, _ARRAYSIZE(szBuffer), L"\r\n");
			WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

			StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"\tindex: %d\r\n", i);
			WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

			StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"\torder: %d\r\n", lpMessage->nOrder);
			WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

			StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"\ttext: %s\r\n", lpMessage->szText);
			WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

			StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"\te-mail: %s\r\n", lpMessage->szEmailAddress);
			WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

			StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"\tcontact: %d\r\n", lpMessage->nContactId);
			WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);
		}
	}

	wcscpy_s(szBuffer, _ARRAYSIZE(szBuffer), L"\r\n");
	WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);
}

static void App_LogWaveDifference(LPWSTR szMessage, INT nMessage, LPWAVE lpLastWave, LPWAVE lpWave)
{
	HANDLE hFile = CreateFile(
		TEXT("dump.txt"),
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwWritten;
		WCHAR szBuffer[1024];

		SetFilePointer(hFile, 0, 0, FILE_END);

		wcscpy_s(szBuffer, _ARRAYSIZE(szBuffer), L"=====================================================================================\r\n");
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		StringCbPrintf(szBuffer, sizeof(WCHAR) * _ARRAYSIZE(szBuffer), L"%s (message %d)\r\n", szMessage, nMessage);
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		wcscpy_s(szBuffer, _ARRAYSIZE(szBuffer), L"Last wave\r\n");
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		App_DumpWave(hFile, lpLastWave);

		wcscpy_s(szBuffer, _ARRAYSIZE(szBuffer), L"Current wave\r\n");
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		App_DumpWave(hFile, lpWave);

		wcscpy_s(szBuffer, _ARRAYSIZE(szBuffer), L"\r\n");
		WriteFile(hFile, szBuffer, wcslen(szBuffer) * sizeof(WCHAR), &dwWritten, NULL);

		CloseHandle(hFile);
	}
}

#endif

static void App_ShowFlyout(HWND hWnd)
{
	LPFLYOUT lpFlyout;
	LPLIST lpUnread;
	POINT p;

	lpUnread = App_GetUnreadWaves();

	Popup_CancelAll();

	g_lpHoveringWave = NULL;

	lpFlyout = Flyout_Create();

	lpFlyout->nTag = (INT_PTR)lpUnread;
	lpFlyout->lpfnWndProc = App_FlyoutWndProc;
	lpFlyout->nWidth = FL_WIDTH;

	if (List_GetCount(lpUnread) == 0)
	{
		lpFlyout->nHeight = FL_BOTTOM_HEIGHT * 2;
	}
	else
	{
		lpFlyout->nHeight = App_CalculateFlyoutHeight(lpUnread);
	}

	GetCursorPos(&p);

	Flyout_Show(lpFlyout, p);
}

static INT App_CalculateFlyoutHeight(LPLIST lpUnread)
{
	RECT r;
	RECT rTmp;
	LPITER lpIter;
	INT i;
	LPUNREAD_WAVE lpWave;
	POINT p;

	p.x = -1;
	p.y = -1;

	r.left = 0;
	r.top = 0;
	r.right = FL_WIDTH - PL_PADDING * 2;
	r.bottom = 32267;

	lpIter = List_Start(lpUnread);
	i = 0;

	while (lpWave = (LPUNREAD_WAVE)List_Next(lpIter))
	{
		if (i > 0)
		{
			r.top += FL_SPACING * 2 + 1;
		}

		rTmp = r;

		lpWave->nHeight = App_PaintUnreadWave(NULL, &rTmp, lpWave, p);
		r.top += lpWave->nHeight;

		i++;

		if (i == FL_MAXIMUM_WAVES)
		{
			break;
		}
	}

	List_End(lpIter);

	return r.top + (PL_PADDING * 2) + FL_BOTTOM_HEIGHT;
}

static LPLIST App_GetUnreadWaves()
{
	LPLIST lpUnreadWaves = List_Create();
	INT nUnreadWaves = 0;
	INT i;
	LPWAVE lpWave;
	LPUNREAD_WAVE lpUnreadWave;
	LPITER lpIter;

	if (g_lpLastWaveResponse == NULL)
	{
		return lpUnreadWaves;
	}

	for (i = 0; i < g_lpLastWaveResponse->nWavesCount; i++)
	{
		lpWave = g_lpLastWaveResponse->lpWaves[i];

		if (lpWave->nUnreadMessages > 0)
		{
			nUnreadWaves++;

			lpUnreadWave = App_CreateUnreadWave(g_lpLastWaveResponse, lpWave, NULL);

			if (lpUnreadWave != NULL)
			{
				List_Add(lpUnreadWaves, lpUnreadWave);
			}
		}
	}

	lpIter = List_Start(lpUnreadWaves);
	i = 1;

	while (lpUnreadWave = (LPUNREAD_WAVE)List_Next(lpIter))
	{
		lpUnreadWave->nCount = nUnreadWaves;
		lpUnreadWave->nIndex = i++;
	}

	List_End(lpIter);

	return lpUnreadWaves;
}

static LRESULT CALLBACK App_FlyoutWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL fOver;

	switch (message)
	{
	case WM_CREATE:
		fOver = FALSE;
		break;

	case WM_PAINT:
		App_FlyoutWmPaint(hWnd);
		break;

	case WM_MOUSEMOVE:
		if (!fOver)
		{
			TRACKMOUSEEVENT tme;

			memset(&tme, 0, sizeof(TRACKMOUSEEVENT));

			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hWnd;

			TrackMouseEvent(&tme);
		}

		App_FlyoutWmMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_MOUSELEAVE:
		App_FlyoutWmMouseMove(hWnd, -1, -1);
		break;

	case WM_SETCURSOR:
		if (g_nFlyoutHitTest != FHT_NONE)
		{ INT i = 0; }
		SetCursor(g_nFlyoutHitTest == FHT_NONE ? g_hCursorArrow : g_hCursorHand);
		return 1;

	case WM_LBUTTONUP:
		App_FlyoutWmLButtonUp(hWnd);
		break;

	case WM_DESTROY:
		g_lpHoveringWave = NULL;
		g_nFlyoutHitTest = FHT_NONE;

		App_DestroyUnreadWaves((LPLIST)Flyout_GetTag(hWnd));
		break;
	}

	return 0;
}

static void App_FlyoutWmPaint(HWND hWnd)
{
	POINT p;
	PAINTSTRUCT ps;

	GetCursorPos(&p);

	ScreenToClient(hWnd, &p);

	BeginPaint(hWnd, &ps);

	App_FlyoutPaint(hWnd, ps.hdc, p, TRUE);

	EndPaint(hWnd, &ps);
}

static void App_FlyoutWmMouseMove(HWND hWnd, WORD x, WORD y)
{
	POINT p;
	HDC hdc;
	
	p.x = x;
	p.y = y;

	hdc = GetDC(hWnd);

	App_FlyoutPaint(hWnd, hdc, p, FALSE);

	DeleteDC(hdc);
}

static void App_FlyoutPaint(HWND hWnd, HDC hdc, POINT p, BOOL fInvalidate)
{
	RECT rClient;
	LPLIST lpUnread = (LPLIST)Flyout_GetTag(hWnd);
	RECT r;
	FLYOUTHITTEST nLastHitTest;

	nLastHitTest = g_nFlyoutHitTest;

	g_nFlyoutHitTest = FHT_NONE;

	//
	// Paint the bottom
	//

	GetClientRect(hWnd, &rClient);

	r = rClient;
	r.top = r.bottom - FL_BOTTOM_HEIGHT;

	App_FlyoutPaintBottom(hdc, &r, p, lpUnread, fInvalidate);

	rClient.bottom -= FL_BOTTOM_HEIGHT;

	if (List_GetCount(lpUnread) > 0)
	{
		App_FlyoutPaintWaves(hWnd, hdc, p, &rClient, lpUnread, fInvalidate);
	}
	else if (fInvalidate)
	{
		App_FlyoutPaintNoWaves(hWnd, hdc, &rClient);
	}
}

static void App_FlyoutPaintWaves(HWND hWnd, HDC hdc, POINT p, LPRECT lpr, LPLIST lpUnread, BOOL fInvalidate)
{
	RECT r;
	LPITER lpIter;
	LPUNREAD_WAVE lpWave;
	HGDIOBJ hOriginal;
	RECT rTmp;
	LPUNREAD_WAVE lpLastUnread;
	INT i;
	BOOL fMouseOver;
	
	static BOOL fInitialised = FALSE;
	static HPEN hBorderPen;

	lpLastUnread = g_lpHoveringWave;
	g_lpHoveringWave = NULL;

	if (!fInitialised)
	{
		hBorderPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	}

	SetBkColor(hdc, GetSysColor(COLOR_WINDOW));

	r = *lpr;

	InflateRect(&r, -PL_PADDING, -PL_PADDING);

	lpIter = List_Start(lpUnread);
	i = 0;

	while (lpWave = (LPUNREAD_WAVE)List_Next(lpIter))
	{
		if (i > 0)
		{
			r.top += FL_SPACING;

			if (fInvalidate)
			{
				hOriginal = SelectPen(hdc, hBorderPen);

				MoveToEx(hdc, lpr->left, r.top, NULL);
				LineTo(hdc, lpr->right, r.top);

				SelectObject(hdc, hOriginal);
			}
			
			r.top += FL_SPACING + 1;
		}

		rTmp = r;
		rTmp.bottom = rTmp.top + lpWave->nHeight;

		fMouseOver = PtInRect(&rTmp, p);

		if (fMouseOver)
		{
			g_lpHoveringWave = lpWave;
			g_nFlyoutHitTest = FHT_WAVE;
		}

		if (
			fInvalidate ||
			(lpLastUnread == lpWave && !fMouseOver) ||
			(lpLastUnread != lpWave && fMouseOver)
		) {
			App_PaintUnreadWave(hdc, &rTmp, lpWave, p);
		}

		r.top += lpWave->nHeight;

		i++;

		if (i == FL_MAXIMUM_WAVES)
		{
			break;
		}
	}

	List_End(lpIter);
}

static void App_FlyoutPaintNoWaves(HWND hWnd, HDC hdc, LPRECT lpr)
{
	HGDIOBJ hOriginal;
	RECT r;
	TEXTMETRIC tm;
	INT nOffset;

	static BOOL fInitialised = FALSE;
	static HBRUSH hBackgroundBrush;

	if (!fInitialised)
	{
		hBackgroundBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	}

	//
	// Draw the background
	//

	r = *lpr;
	r.bottom = r.top + FL_BOTTOM_HEIGHT;

	FillRect(hdc, &r, hBackgroundBrush);

	//
	// Draw the label
	//

	memset(&tm, 0, sizeof(TEXTMETRIC));

	GetTextMetrics(hdc, &tm);

	nOffset = ((r.bottom - r.top) - tm.tmHeight) / 2;

	r.top += nOffset;
	r.left += nOffset;
	r.right -= nOffset;

	SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
	SetBkColor(hdc, GetSysColor(COLOR_WINDOW));

	hOriginal = SelectFont(hdc, GetMessageBoxFont());

	DrawText(hdc, L"There are no unread waves", -1, &r, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	SelectObject(hdc, hOriginal);
}

static INT App_FlyoutPaintBottom(HDC hdc, LPRECT lpr, POINT p, LPLIST lpUnread, BOOL fInvalidate)
{
	RECT r;
	HGDIOBJ hOriginal;
	TEXTMETRIC tm;
	INT nOffset;
	WCHAR szBuffer[200];
	RECT r2;
	INT nCount;

	static BOOL fInitialised = FALSE;
	static HPEN hBorderPen;
	static HBRUSH hBackgroundBrush;
	static HFONT hFont;

	if (!fInitialised)
	{
		fInitialised = TRUE;

		hBorderPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
		hBackgroundBrush = GetSysColorBrush(COLOR_BTNFACE);
		hFont = App_GetUrlFont();
	}

	//
	// Draw the background
	//

	if (fInvalidate)
	{
		r = *lpr;

		r.top = r.bottom - FL_BOTTOM_HEIGHT;

		hOriginal = SelectPen(hdc, hBorderPen);

		MoveToEx(hdc, r.left, r.top, NULL);
		LineTo(hdc, r.right, r.top);

		SelectObject(hdc, hOriginal);

		r.top += 1;

		FillRect(hdc, &r, hBackgroundBrush);

		SelectObject(hdc, hOriginal);
	}

	//
	// Draw the label
	//

	memset(&tm, 0, sizeof(TEXTMETRIC));

	GetTextMetrics(hdc, &tm);

	r = *lpr;
	r.top = r.bottom - FL_BOTTOM_HEIGHT + 1;

	nOffset = ((r.bottom - r.top) - tm.tmHeight) / 2;

	r.top += nOffset;
	r.left += nOffset;
	r.right -= nOffset;

	hOriginal = SelectFont(hdc, hFont);

	DrawText(hdc, L"Go to Inbox", -1, &r, DT_CALCRECT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

	if (fInvalidate)
	{
		SetTextColor(hdc, RGB(0, 0, 255));
		SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));

		DrawText(hdc, L"Go to Inbox", -1, &r, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE);

		nCount = List_GetCount(lpUnread) - FL_MAXIMUM_WAVES;

		if (nCount > 0)
		{
			if (nCount == 1)
			{
				wcscpy_s(szBuffer, _ARRAYSIZE(szBuffer), L"1 more unread wave");
			}
			else
			{
				StringCbPrintf(szBuffer, _ARRAYSIZE(szBuffer), L"%d more unread waves", nCount);
			}

			r2 = r;

			r2.left = r2.right;
			r2.right = lpr->right - nOffset;

			SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
			SelectFont(hdc, GetMessageBoxFont());

			DrawText(hdc, szBuffer, -1, &r2, DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE | DT_RIGHT);
		}
	}

	InflateRect(&r, 2, 2);

	if (PtInRect(&r, p))
	{
		g_nFlyoutHitTest = FHT_INBOX;
	}

	SelectObject(hdc, hOriginal);

	return FL_BOTTOM_HEIGHT;
}

static HFONT App_GetUrlFont()
{
	LOGFONT lf;

	GetObject(GetMessageBoxFont(), sizeof(LOGFONT), &lf);

	lf.lfUnderline = TRUE;

	return CreateFontIndirect(&lf);
}

static void App_FlyoutWmLButtonUp(HWND hWnd)
{
	switch (g_nFlyoutHitTest)
	{
	case FHT_INBOX:
		Wave_OpenInbox(g_lpWaveSession);
		break;

	case FHT_WAVE:
		Wave_OpenWave(g_lpWaveSession, g_lpHoveringWave->szId);
		break;

	default:
		return;
	}

	DestroyWindow(hWnd);
}

static void App_DestroyUnreadWaves(LPLIST lpUnread)
{
	LPUNREAD_WAVE lpWave;

	while (lpWave = (LPUNREAD_WAVE)List_Dequeue(lpUnread))
	{
		App_DestroyUnreadWave(lpWave);
	}

	List_Destroy(lpUnread);
}
