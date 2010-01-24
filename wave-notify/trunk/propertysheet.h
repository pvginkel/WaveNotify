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

#ifndef _INC_PROPERTYSHEET
#define _INC_PROPERTYSHEET

#pragma once

class CPropertySheet;
class CPropertySheetPage;

class CModelessPropertySheets
{
public:
	typedef map<INT, CPropertySheet *> THandlesMap;
	typedef THandlesMap::iterator THandlesMapIter;
	typedef THandlesMap::const_iterator THandlesMapConstIter;

private:
	static THandlesMap m_vSheets;

public:
	static BOOL IsDialogMessage(LPMSG lpMsg);
	static void RegisterSheet(INT nType, CPropertySheet * lpDialog);
	static void UnregisterSheet(INT nType);
	static void UnregisterSheet(CPropertySheet * lpDialog);
	static BOOL ContainsType(INT nType);
	static CPropertySheet * GetSheet(INT nType) { return m_vSheets[nType]; }
};

typedef vector<CPropertySheetPage *> TPropertySheetPagesVector;
typedef TPropertySheetPagesVector::iterator TPropertySheetPagesVectorIter;
typedef TPropertySheetPagesVector::const_iterator TPropertySheetPagesVectorConstIter;

class CPropertySheet : public CWindowHandle
{
private:
	DWORD m_dwFlags;
	HICON m_hIcon;
	wstring m_szCaption;
	UINT m_uStartPage;
	TPropertySheetPagesVector m_vPages;
	LPPROPSHEETHEADER m_lpSheet;

	static CPropertySheet * m_lpCreatingPropertySheet;

public:
	CPropertySheet();
	virtual ~CPropertySheet();
	INT_PTR ShowDialog(CWindowHandle * lpParentWindow = NULL);
	INT_PTR Create(INT nType, CWindowHandle * lpParentWindow = NULL);

	void AddPage(CPropertySheetPage * lpPage);
	const TPropertySheetPagesVector & GetPages() const { return m_vPages; }

	DWORD GetFlags() const { return m_dwFlags; }
	void SetFlags(DWORD dwFlags) { m_dwFlags = dwFlags; }
	HICON GetIcon() const { return m_hIcon; }
	void SetIcon(HICON hIcon) {
		CHECK_HANDLE(hIcon);

		m_hIcon = hIcon;
	}
	wstring GetCaption() const { return m_szCaption; }
	void SetCaption(wstring szCaption) {
		CHECK_NOT_EMPTY(szCaption);

		m_szCaption = szCaption;
	}
	UINT GetStartPage() const { return m_uStartPage; }
	void SetStartPage(UINT uStartPage) { m_uStartPage = uStartPage; }

protected:
	virtual void OnCreated() { }

private:
	static int CALLBACK SheetProcCallback(HWND hWnd, UINT uMsg, LPARAM lParam);

	LPPROPSHEETHEADER BuildStructures(CWindowHandle * lpParentWindow);
	INT SheetProc(UINT uMsg);
};

class CPropertySheetPage : public CWindowHandle
{
private:
	DWORD m_dwFlags;
	UINT m_uResource;
	wstring m_szTitle;

public:
	CPropertySheetPage();

	DWORD GetFlags() const { return m_dwFlags; }
	void SetFlags(DWORD dwFlags) { m_dwFlags = dwFlags; }
	UINT GetResource() const { return m_uResource; }
	void SetResource(UINT uResource) { m_uResource = uResource; }
	wstring GetTitle() const { return m_szTitle; }
	void SetTitle(wstring szTitle) {
		CHECK_NOT_EMPTY(szTitle);

		m_szTitle = szTitle;
	}

protected:
	virtual INT_PTR DialogProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnFocus() { return FALSE; }
	virtual BOOL OnApply() { return FALSE; }
	virtual BOOL OnHelp() { return FALSE; }
	virtual BOOL OnLeave() { return FALSE; }
	virtual BOOL OnQueryCancel() { return FALSE; }
	virtual BOOL OnQueryInitialFocus() { return FALSE; }
	virtual BOOL OnReset() { return FALSE; }
	virtual BOOL OnCommand(WORD wCommand, WORD wNotifyMessage) { return FALSE; }

	void InitializeStructure(LPPROPSHEETPAGE lpPage);
	void SetStateValid() {
		SetMessageResult(PSNRET_NOERROR);
	}
	void SetStateInvalid(BOOL fKeepCurrentPage = TRUE) {
		SetMessageResult(fKeepCurrentPage ? PSNRET_INVALID_NOCHANGEPAGE : PSNRET_INVALID);
	}

	friend class CPropertySheet;

private:
	static INT_PTR CALLBACK DialogProcCallback(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};

#endif // _INC_PROPERTYSHEET
