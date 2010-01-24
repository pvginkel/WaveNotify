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
#include "browser.h"

const wstring CBrowser::BrowserDefault(L"default");
const wstring CBrowser::BrowserChrome(L"chrome");
const wstring CBrowser::BrowserFirefox(L"firefox");
const wstring CBrowser::BrowserOpera(L"opera");
const wstring CBrowser::BrowserSafari(L"safari");
const wstring CBrowser::BrowserInternetExplorer(L"iexplore");

void CBrowser::OpenUrl(wstring szBrowser, wstring szUrl)
{
	if (RunningWine())
	{
		OpenUrlForWine(szUrl);
		return;
	}

	if (szBrowser != BrowserDefault)
	{
		wstring szExecutable(GetBrowserExecutable(szBrowser));

		if (!szExecutable.empty())
		{
			OpenUrlWithExecutable(szExecutable, szUrl);
			return;
		}
	}

	OpenUrlWithDefault(szUrl);
}

void CBrowser::OpenUrlForWine(wstring szUrl)
{
	WCHAR szSystemPath[MAX_PATH];

	SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, 0, szSystemPath);

	wstring szPath(szSystemPath);
	
	szPath += L"\\winebrowser.exe";

	OpenUrlWithExecutable(szPath, szUrl);
}

void CBrowser::OpenUrlWithExecutable(wstring szExecutable, wstring szUrl)
{
	ASSERT(!szExecutable.empty() && !szUrl.empty());

	wstring szCommandLine(Format(L"\"%s\" \"%s\"", szExecutable.c_str(), szUrl.c_str()));

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(PROCESS_INFORMATION);

	LPWSTR szMutableCommandLine = (LPWSTR)malloc(sizeof(WCHAR) * (szCommandLine.size() + 1));

	wcscpy(szMutableCommandLine, szCommandLine.c_str());

	CreateProcess(
		szExecutable.c_str(),
		szMutableCommandLine,
		NULL,
		NULL,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE,
		NULL,
		NULL,
		&si,
		&pi);

	free(szMutableCommandLine);
}

void CBrowser::OpenUrlWithDefault(wstring szUrl)
{
	ASSERT(!szUrl.empty());

	ShellExecute(NULL, L"open", szUrl.c_str(), NULL, NULL, SW_SHOW);
}

BOOL CBrowser::IsBrowserAvailable(wstring szBrowser)
{
	if (szBrowser == BrowserDefault)
	{
		return TRUE;
	}
	else if (RunningWine())
	{
		return FALSE;
	}
	else
	{
		wstring szExecutable(GetBrowserExecutable(szBrowser));

		if (szExecutable.empty())
		{
			return FALSE;
		}
		else
		{
			return GetFileAttributes(szExecutable.c_str()) != INVALID_FILE_ATTRIBUTES;
		}
	}
}

void CBrowser::GetAvailableBrowsers(TStringVector & vBrowsers)
{
	vBrowsers.clear();

	vBrowsers.push_back(BrowserDefault);

	if (IsBrowserAvailable(BrowserChrome))
	{
		vBrowsers.push_back(BrowserChrome);
	}

	if (IsBrowserAvailable(BrowserFirefox))
	{
		vBrowsers.push_back(BrowserFirefox);
	}

	if (IsBrowserAvailable(BrowserOpera))
	{
		vBrowsers.push_back(BrowserOpera);
	}

	if (IsBrowserAvailable(BrowserSafari))
	{
		vBrowsers.push_back(BrowserSafari);
	}

	if (IsBrowserAvailable(BrowserInternetExplorer))
	{
		vBrowsers.push_back(BrowserInternetExplorer);
	}
}

wstring CBrowser::GetBrowserExecutable(wstring szBrowser)
{
	if (szBrowser == BrowserChrome)
	{
		return GetChromeExecutable();
	}
	else if (szBrowser == BrowserOpera)
	{
		return GetOperaExecutable();
	}
	else if (szBrowser == BrowserSafari)
	{
		return GetSafariExecutable();
	}
	else if (szBrowser == BrowserInternetExplorer)
	{
		return GetInternetExplorerExecutable();
	}
	else if (szBrowser == BrowserFirefox)
	{
		return GetFirefoxExecutable();
	}
	else
	{
		return L"";
	}
}

wstring CBrowser::GetBrowserDescription(wstring szBrowser)
{
	if (szBrowser == BrowserDefault)
	{
		return L"Default browser";
	}
	else if (szBrowser == BrowserChrome)
	{
		return L"Google Chrome";
	}
	else if (szBrowser == BrowserOpera)
	{
		return L"Opera";
	}
	else if (szBrowser == BrowserSafari)
	{
		return L"Safari";
	}
	else if (szBrowser == BrowserInternetExplorer)
	{
		return L"Internet Explorer";
	}
	else if (szBrowser == BrowserFirefox)
	{
		return L"Mozilla Firefox";
	}
	else
	{
		return L"";
	}
}

wstring CBrowser::GetChromeExecutable()
{
	wstring szResult(GetExecutableFromStartMenu(L"chrome.exe"));

	if (szResult.empty())
	{
		return szResult;
	}
	else
	{
		return GetExecutableFromLocalSettings(L"\\Google\\Chrome\\Application\\chrome.exe");
	}
}

wstring CBrowser::GetOperaExecutable()
{
	wstring szResult(GetExecutableFromStartMenu(L"opera.exe"));

	if (szResult.empty())
	{
		return szResult;
	}
	else
	{
		return GetExecutableFromProgramFiles(L"\\Opera\\opera.exe");
	}
}

wstring CBrowser::GetSafariExecutable()
{
	wstring szResult(GetExecutableFromStartMenu(L"Safari.exe"));

	if (szResult.empty())
	{
		return szResult;
	}
	else
	{
		return GetExecutableFromProgramFiles(L"\\Safari\\Safari.exe");
	}
}

wstring CBrowser::GetInternetExplorerExecutable()
{
	wstring szResult(GetExecutableFromStartMenu(L"iexplore.exe"));

	if (szResult.empty())
	{
		return szResult;
	}
	else
	{
		return GetExecutableFromProgramFiles(L"\\Internet Explorer\\iexplore.exe");
	}
}

wstring CBrowser::GetFirefoxExecutable()
{
	wstring szResult(GetExecutableFromStartMenu(L"firefox.exe"));

	if (szResult.empty())
	{
		return szResult;
	}
	else
	{
		return GetExecutableFromProgramFiles(L"\\Mozilla Firefox\\firefox.exe");
	}
}

wstring CBrowser::GetExecutableFromProgramFiles(wstring szRelativePath)
{
	ASSERT(!szRelativePath.empty());

	WCHAR szPath[MAX_PATH];

	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAM_FILES, FALSE))
	{
		wstring szExecPath(szPath);

		szExecPath += szRelativePath;

		if (GetFileAttributes(szExecPath.c_str()) != INVALID_FILE_ATTRIBUTES)
		{
			return szExecPath;
		}
	}

	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAM_FILESX86, FALSE))
	{
		wstring szExecPath(szPath);

		szExecPath += szRelativePath;

		if (GetFileAttributes(szExecPath.c_str()) != INVALID_FILE_ATTRIBUTES)
		{
			return szExecPath;
		}
	}

	return L"";
}

wstring CBrowser::GetExecutableFromLocalSettings(wstring szRelativePath)
{
	ASSERT(!szRelativePath.empty());

	WCHAR szPath[MAX_PATH];

	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, FALSE))
	{
		wstring szExecPath(szPath);

		szExecPath += szRelativePath;

		if (GetFileAttributes(szExecPath.c_str()) != INVALID_FILE_ATTRIBUTES)
		{
			return szExecPath;
		}
	}

	return L"";
}

wstring CBrowser::GetExecutableFromStartMenu(wstring szExecutable)
{
	ASSERT(!szExecutable.empty());

	CRegKey * lpKey = CRegKey::OpenKey(HKEY_CURRENT_USER, L"Software\\Clients\\StartMenuInternet");

	if (lpKey != NULL)
	{
		wstring szResult(GetExecutableFromStartMenu(lpKey, szExecutable));

		delete lpKey;

		if (!szResult.empty())
		{
			return szResult;
		}
	}

	lpKey = CRegKey::OpenKey(HKEY_LOCAL_MACHINE, L"Software\\Clients\\StartMenuInternet");

	if (lpKey != NULL)
	{
		wstring szResult(GetExecutableFromStartMenu(lpKey, szExecutable));

		delete lpKey;

		if (!szResult.empty())
		{
			return szResult;
		}
	}

	return L"";
}

wstring CBrowser::GetExecutableFromStartMenu(CRegKey * lpKey, wstring szExecutable)
{
	ASSERT(lpKey != NULL && !szExecutable.empty());
	
	TStringVector vSubKeys;
	
	if (!lpKey->GetSubKeys(vSubKeys))
	{
		return L"";
	}

	for (TStringVectorIter iter = vSubKeys.begin(); iter != vSubKeys.end(); iter++)
	{
		CRegKey * lpSubKey = CRegKey::OpenKey(lpKey, *iter);

		if (lpSubKey != NULL)
		{
			wstring szResult(GetExecutableFromStartMenuKey(lpSubKey, szExecutable));

			delete lpSubKey;

			if (!szResult.empty())
			{
				return szResult;
			}
		}
	}

	return L"";
}

wstring CBrowser::GetExecutableFromStartMenuKey(CRegKey * lpKey, wstring szExecutable)
{
	ASSERT(lpKey != NULL && !szExecutable.empty());

	CRegKey * lpSubKey = CRegKey::OpenKey(lpKey, L"shell\\open\\command");
	wstring szPath;

	if (lpSubKey != NULL)
	{
		if (lpSubKey->GetValue(L"", szPath))
		{
			if (szPath[0] == L'"')
			{
				UINT nPos = szPath.find(L'"', 1);

				if (nPos != wstring::npos)
				{
					szPath = szPath.substr(1, nPos - 1);
				}
			}
		}

		delete lpSubKey;
	}

	if (!szPath.empty() && _wcsicmp(GetBasename(szPath).c_str(), szExecutable.c_str()) == 0)
	{
		return szPath;
	}
	else
	{
		return L"";
	}
}
