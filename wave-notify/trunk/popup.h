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

#ifndef _INC_POPUP
#define _INC_POPUP

#pragma once

typedef enum
{
	PS_PENDING,
	PS_OPENING,
	PS_WAITING,
	PS_CLOSING
} POPUP_STATE;

class CPopup;

typedef vector<CPopup *> TPopupVector;
typedef TPopupVector::iterator TPopupVectorIter;
typedef TPopupVector::const_iterator TPopupVectorConstIter;

class CPopupWindow : public CWindow
{
private:
	static CPopupWindow * m_lpInstance;
	TPopupVector m_vQueue;
	TASKBAR_LOCATION_EDGE m_uTaskbarEdge;
	POPUP_STATE m_nState;
	BOOL m_fMouseOver;
	INT m_nAnimationStep;
	CPopup * m_lpCurrent;
	CTimer * m_lpPopupTimer;
	CTimer * m_lpMouseOverTimer;
	CTimer * m_lpIdleCompleteTimer;

public:
	CPopupWindow();
	~CPopupWindow();

	void Show(CPopup * lpPopup);
	LRESULT DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

	void CancelAll();
	void CancelPopup(CPopup * lpPopup);
	void ShowNext();
	void GetPopups(TPopupVector & vPopups) const;
	CPopup * GetCurrent() const { return m_lpCurrent; }
	POPUP_STATE GetState() const { return m_nState; }
	void ExtendPopupDuration(INT nDuration);

	static CPopupWindow * Instance(BOOL fCreate = FALSE);

protected:
	ATOM CreateClass(LPWNDCLASSEX lpWndClass);
	HWND CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu);
	LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	void CalculatePopupRect();
	BOOL ShouldShowPopup();
	void ShowPopup();
	void DequeuePopup();
	void ProcessPopupTimer();
	void ProcessMouseOverTimer();
	void ProcessIdleCompleteTimer();
	void AnimatePending();
	void AnimateOpening();
	void AnimateWaiting();
	void AnimateClosing();
	void OpeningComplete();
	void CompleteClosing();
	void UpdateFromAnimationStep();

	LRESULT OnMouseMove();
	LRESULT OnMouseLeave();
};

class CPopup
{
private:
	INT m_nDuration;
	INT m_nHeight;
	INT m_nWidth;
	INT m_nAnimationDuration;
	RECT m_rcLocation;

public:
	CPopup();
	virtual ~CPopup() { }

	INT GetDuration() const { return m_nDuration; }
	void SetDuration(INT nDuration) {
		CHECK_GT_0(nDuration);

		m_nDuration = nDuration;
	}
	INT GetHeight() const { return m_nHeight; }
	void SetHeight(INT nHeight) {
		CHECK_GT_0(nHeight);

		m_nHeight = nHeight;
	}
	INT GetWidth() const { return m_nWidth; }
	void SetWidth(INT nWidth) {
		CHECK_GT_0(nWidth);

		m_nWidth = nWidth;
	}
	INT GetAnimationDuration() const { return m_nAnimationDuration; }
	void SetAnimationDuration(INT nAnimationDuration) {
		CHECK_GT_0(nAnimationDuration);

		m_nAnimationDuration = nAnimationDuration;
	}
	void GetLocation(LPRECT lpRect) const {
		ASSERT(lpRect != NULL);

		memcpy(lpRect, &m_rcLocation, sizeof(RECT));
	}
	void SetLocation(LPRECT lpRect) {
		ASSERT(lpRect != NULL);

		memcpy(&m_rcLocation, lpRect, sizeof(RECT));
	}
	CPopupWindow * GetWindow() const {
		CPopupWindow * lpResult = CPopupWindow::Instance();
		ASSERT(lpResult != NULL);
		return lpResult;
	}
	void ExtendDuration(INT nDuration);

	void Show();
	void Cancel() { GetWindow()->CancelPopup(this); }
	BOOL IsDisplaying() const {
		CPopupWindow * lpWindow = GetWindow();

		return lpWindow->GetCurrent() == this && lpWindow->GetState() != PS_PENDING;
	}

protected:
	virtual LRESULT WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

	friend CPopupWindow;
};

#endif // _INC_POPUP
