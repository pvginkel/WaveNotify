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

#ifndef _INC_NOTIFYICON
#define _INC_NOTIFYICON

#pragma once

class CNotifyIcon
{
private:
	UINT m_uId;
	CWindow * m_lpWindow;
	wstring m_szTooltip;
	HICON m_hIcon;

public:
	CNotifyIcon(CWindow * lpWindow, UINT uId, wstring szTooltip, HICON hIcon);
	virtual ~CNotifyIcon();

	CWindow * GetWindow() const { return m_lpWindow; }
	UINT GetID() const { return m_uId; }
	void SetWindow(CWindow * lpWindow) {
		ASSERT(lpWindow != NULL);

		m_lpWindow = lpWindow;
	}
	wstring GetTooltip() const { return m_szTooltip; }
	void SetTooltip(wstring szTooltip) {
		m_szTooltip = szTooltip;
		Update();
	}
	HICON GetIcon() const { return m_hIcon; }
	void SetIcon(HICON hIcon) {
		CHECK_HANDLE(hIcon);

		m_hIcon = hIcon;
		Update();
	}
	void Recreate();
	void Create();
	void Destroy();

private:
	void Update();
	void PopulateData(PNOTIFYICONDATA lpnid);
};

#endif // _INC_NOTIFYICON
