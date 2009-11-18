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

static INT_PTR CALLBACK LoginForm_DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static INT_PTR LoginForm_WmCommand(HWND hWnd, WORD wCommand, WORD wNotifyMessage);
static INT_PTR LoginForm_WmInitDialog(HWND hWnd);
static INT_PTR LoginForm_ReadRegistry(HWND hWnd);
static INT_PTR LoginForm_ProcessLogin(HWND hWnd);
static INT_PTR LoginForm_UpdateEnabled(HWND hWnd);
static void LoginForm_UpdateRegistry(HWND hWnd);

INT_PTR LoginForm_ShowDialog(HWND hWnd)
{
	return DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_LOGIN), hWnd, LoginForm_DialogProc);
}

static INT_PTR CALLBACK LoginForm_DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return LoginForm_WmInitDialog(hWnd);

	case WM_COMMAND:
		return LoginForm_WmCommand(hWnd, LOWORD(wParam), HIWORD(wParam));
	}

	return (INT_PTR)FALSE;
}

static INT_PTR LoginForm_WmInitDialog(HWND hWnd)
{
	static HICON hIcon = NULL;
	static HICON hIconSmall = NULL;

	if (hIcon == NULL)
	{
		hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	}
	if (hIconSmall == NULL)
	{
	 	hIconSmall = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAINICON_SMALL));
	}

	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);

	return LoginForm_ReadRegistry(hWnd);
}

static INT_PTR LoginForm_ReadRegistry(HWND hWnd)
{
	WCHAR szBuffer[REGVALUE_SIZE];
	HKEY hBaseKey = NULL;
	DWORD dwType;
	DWORD dwLength;
	DWORD dwValue;
	BOOL fHasUsername = FALSE;
	BOOL fHasPassword = FALSE;

	if (RegOpenKey(HKEY_CURRENT_USER, REG_BASEKEY, &hBaseKey) != ERROR_SUCCESS)
	{
		goto __end;
	}

	dwLength = _ARRAYSIZE(szBuffer);

	if (
		RegQueryValueEx(hBaseKey, REG_USERNAME, NULL, &dwType, (LPBYTE)szBuffer, &dwLength) == ERROR_SUCCESS &&
		dwType == REG_SZ)
	{
		SetDlgItemText(hWnd, IDC_USERNAME, szBuffer);

		fHasUsername = wcslen(szBuffer) != 0;
	}

	dwLength = _ARRAYSIZE(szBuffer);

	if (
		RegQueryValueEx(hBaseKey, REG_PASSWORD, NULL, &dwType, (LPBYTE)szBuffer, &dwLength) == ERROR_SUCCESS &&
		dwType == REG_SZ)
	{
		SetDlgItemText(hWnd, IDC_PASSWORD, szBuffer);

		fHasPassword = wcslen(szBuffer) != 0;
	}

	dwLength = sizeof(dwValue);

	if (
		RegQueryValueEx(hBaseKey, REG_REMEMBER_PASSWORD, NULL, &dwType, (LPBYTE)&dwValue, &dwLength) == ERROR_SUCCESS &&
		dwType == REG_DWORD)
	{
		SendDlgItemMessage(hWnd, IDC_REMEMBER_PASSWORD, BM_SETCHECK, dwValue != 0 ? BST_CHECKED : BST_UNCHECKED, 0);
	}

__end:
	if (fHasUsername)
	{
		if (fHasPassword)
		{
			SetFocus(GetDlgItem(hWnd, IDOK));
		}
		else
		{
			SetFocus(GetDlgItem(hWnd, IDC_PASSWORD));
		}
	}
	else
	{
		SetFocus(GetDlgItem(hWnd, IDC_USERNAME));
	}

	if (hBaseKey != NULL)
	{
		RegCloseKey(hBaseKey);
	}

	return (INT_PTR)FALSE;
}

static INT_PTR LoginForm_WmCommand(HWND hWnd, WORD wCommand, WORD wNotifyMessage)
{
	switch (wCommand)
	{
	case IDOK:
		return LoginForm_ProcessLogin(hWnd);

	case IDCANCEL:
		EndDialog(hWnd, IDCANCEL);
		return (INT_PTR)TRUE;

	case IDC_USERNAME:
	case IDC_PASSWORD:
		switch (wNotifyMessage)
		{
		case EN_CHANGE:
			return LoginForm_UpdateEnabled(hWnd);
		}
		return (INT_PTR)FALSE;
	}

	return (INT_PTR)FALSE;
}

static INT_PTR LoginForm_ProcessLogin(HWND hWnd)
{
	WCHAR szUsername[REGVALUE_SIZE];
	WCHAR szPassword[REGVALUE_SIZE];

	GetDlgItemText(hWnd, IDC_USERNAME, szUsername, _ARRAYSIZE(szUsername));
	GetDlgItemText(hWnd, IDC_PASSWORD, szPassword, _ARRAYSIZE(szUsername));

	if (g_lpWaveSession != NULL)
	{
		if (Wave_IsLoggedOn(g_lpWaveSession))
		{
			Wave_Logoff(g_lpWaveSession);
		}

		Wave_DestroySession(g_lpWaveSession);
		g_lpWaveSession = NULL;
	}

	g_lpWaveSession = Wave_CreateSession(szUsername, szPassword);

	Wave_Login(g_lpWaveSession);

	if (Wave_IsLoggedOn(g_lpWaveSession))
	{
		LoginForm_UpdateRegistry(hWnd);

		EndDialog(hWnd, IDOK);
	}
	else
	{
		MessageBox(
			hWnd,
			L"Incorrect username or password",
			L"Google Wave Notifier",
			MB_ICONERROR | MB_OK);
	}

	return (INT_PTR)TRUE;
}

static INT_PTR LoginForm_UpdateEnabled(HWND hWnd)
{
	BOOL fHasUsername;
	BOOL fHasPassword;
	WCHAR szBuffer[REGVALUE_SIZE];

	GetDlgItemText(hWnd, IDC_USERNAME, szBuffer, _ARRAYSIZE(szBuffer));

	fHasUsername = wcslen(szBuffer) != 0;

	GetDlgItemText(hWnd, IDC_PASSWORD, szBuffer, _ARRAYSIZE(szBuffer));

	fHasPassword = wcslen(szBuffer) != 0;

	EnableWindow(GetDlgItem(hWnd, IDOK), fHasUsername && fHasPassword);

	SendMessage(hWnd, DM_SETDEFID, IDOK, 0);

	return (INT_PTR)TRUE;
}

static void LoginForm_UpdateRegistry(HWND hWnd)
{
	HKEY hBaseKey = NULL;
	WCHAR szBuffer[REGVALUE_SIZE];
	DWORD dwRemember;

	if (RegCreateKey(HKEY_CURRENT_USER, REG_BASEKEY, &hBaseKey) != ERROR_SUCCESS)
	{
		return;
	}

	GetDlgItemText(hWnd, IDC_USERNAME, szBuffer, _ARRAYSIZE(szBuffer));

	RegSetValueEx(hBaseKey, REG_USERNAME, 0, REG_SZ, (LPBYTE)szBuffer, sizeof(WCHAR) * wcslen(szBuffer));

	dwRemember = SendDlgItemMessage(hWnd, IDC_REMEMBER_PASSWORD, BM_GETCHECK, 0, 0) == BST_CHECKED;

	RegSetValueEx(hBaseKey, REG_REMEMBER_PASSWORD, 0, REG_DWORD, (LPBYTE)&dwRemember, sizeof(DWORD));

	if (dwRemember)
	{
		GetDlgItemText(hWnd, IDC_PASSWORD, szBuffer, _ARRAYSIZE(szBuffer));

		RegSetValueEx(hBaseKey, REG_PASSWORD, 0, REG_SZ, (LPBYTE)szBuffer, sizeof(WCHAR) * wcslen(szBuffer));
	}
	else
	{
		RegDeleteValue(hBaseKey, REG_PASSWORD);
	}

	if (hBaseKey != NULL)
	{
		RegCloseKey(hBaseKey);
	}
}
