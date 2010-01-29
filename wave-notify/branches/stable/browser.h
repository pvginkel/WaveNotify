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

#ifndef _INC_BROWSER
#define _INC_BROWSER

class CBrowser
{
public:
	static const wstring BrowserDefault;
	static const wstring BrowserChrome;
	static const wstring BrowserOpera;
	static const wstring BrowserSafari;
	static const wstring BrowserInternetExplorer;
	static const wstring BrowserFirefox;

	static BOOL IsBrowserAvailable(wstring szBrowser);
	static void GetAvailableBrowsers(TStringVector & vBrowsers);
	static wstring GetBrowserExecutable(wstring szBrowser);
	static wstring GetBrowserDescription(wstring szBrowser);
	static void OpenUrl(wstring szBrowser, wstring szUrl);

	static void OpenUrlForWine(wstring szUrl);
	static void OpenUrlWithExecutable(wstring szExecutable, wstring szUrl);
	static void OpenUrlWithDefault(wstring szUrl);

	static wstring GetChromeExecutable();
	static wstring GetOperaExecutable();
	static wstring GetSafariExecutable();
	static wstring GetInternetExplorerExecutable();
	static wstring GetFirefoxExecutable();

private:
	static wstring GetExecutableFromProgramFiles(wstring szPath);
	static wstring GetExecutableFromLocalSettings(wstring szPath);
	static wstring GetExecutableFromStartMenu(wstring szExecutable);
	static wstring GetExecutableFromStartMenu(CRegKey * lpKey, wstring szExecutable);
	static wstring GetExecutableFromStartMenuKey(CRegKey * lpKey, wstring szExecutable);
	static wstring GetExecutableFromClassesRoot(wstring szKey);
	static wstring GetPathFromShellKey(CRegKey * lpKey);
};

#endif // _INC_BROWSER
