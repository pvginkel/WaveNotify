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

#ifndef _INC_POPUPS
#define _INC_POPUPS

#pragma once

#include "wave.h"

typedef enum
{
	PT_MESSAGE,
	PT_WAVE,
	PT_CONTACT_ONLINE
} POPUP_TYPE;

class CPopupBase : public CPopup
{
private:
	POPUP_TYPE m_nType;
	BOOL m_fEnableCloseButton;
	BOOL m_fPaintIcon;
	RECT m_rcCloseButtonRect;

protected:
	CPopupBase(POPUP_TYPE nType) {
		m_nType = nType;
		m_fEnableCloseButton = TRUE;
		m_fPaintIcon = TRUE;
		memset(&m_rcCloseButtonRect, 0, sizeof(RECT));
	}

	void PaintBackground(CDC & dc);
	BOOL GetEnableCloseButton() const { return m_fEnableCloseButton; }
	void SetEnableCloseButton(BOOL fValue) { m_fEnableCloseButton = fValue; }
	BOOL GetPaintIcon() const { return m_fPaintIcon; }
	void SetPaintIcon(BOOL fValue) { m_fPaintIcon = fValue; }
	BOOL HitTestCloseButton(POINT pt);

public:
	POPUP_TYPE GetType() const { return m_nType; }
};

class CMessagePopup : public CPopupBase
{
private:
	wstring m_szMessage;
	wstring m_szUrl;

public:
	CMessagePopup(wstring szMessage);

	wstring GetUrl() const { return m_szUrl; }
	void SetUrl(wstring szUrl) { m_szUrl = szUrl; }

protected:
	LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	LRESULT OnPaint();
	LRESULT OnLeftButtonUp(LPARAM lParam);
};

class CUnreadWavePopup : public CPopupBase
{
private:
	CUnreadWave * m_lpWave;
	UINT m_uIndex;
	UINT m_uCount;

public:
	CUnreadWavePopup(CUnreadWave * lpWave, UINT uIndex = 0, UINT uCount = 0);
	~CUnreadWavePopup();

	CUnreadWave * GetUnread() const { return m_lpWave; }
	void UpdateUnread(CUnreadWave * lpUnread);
	void ContactsUpdated(CWaveContactCollection * lpContacts);
	void SetCountIndex(UINT uIndex, UINT uCount) {
		m_uIndex = uIndex;
		m_uCount = uCount;
		InvalidateRect(GetHandle(), NULL, FALSE);
	}
	UINT GetCount() const { return m_uCount; }
	UINT GetIndex() const { return m_uIndex; }

protected:
	LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	LRESULT OnPaint();
	LRESULT OnLeftButtonUp(LPARAM lParam);
};

class CContactOnlinePopup : public CPopupBase
{
private:
	CWaveContact * m_lpContact;
	BOOL m_fOnline;

public:
	CContactOnlinePopup(CWaveContact * lpContact, BOOL fOnline);
	~CContactOnlinePopup() { }

	CWaveContact * GetContact() const { return m_lpContact; }
	BOOL GetOnline() const { return m_fOnline; }

protected:
	LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	LRESULT OnPaint();
	LRESULT OnLeftButtonUp(LPARAM lParam);
};

#endif // _INC_POPUPS
