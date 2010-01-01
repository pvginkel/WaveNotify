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

#define ANIMATION_STEPS			20
#define IDLE_TIME			2000

CPopupWindow * CPopupWindow::m_lpInstance = NULL;

CPopupWindow::CPopupWindow() : CWindow(L"Popup")
{
	ASSERT(m_lpInstance == NULL);

	m_lpInstance = this;
	m_lpCurrent = NULL;
	m_fMouseOver = FALSE;
	m_nAnimationStep = 0;

	Create();

	m_lpPopupTimer = new CTimer();

	m_lpPopupTimer->Tick += AddressOf<CPopupWindow>(this, &CPopupWindow::ProcessPopupTimer);

	m_lpMouseOverTimer = new CTimer(200);

	m_lpMouseOverTimer->Tick += AddressOf<CPopupWindow>(this, &CPopupWindow::ProcessMouseOverTimer);

	m_lpIdleCompleteTimer = new CTimer(TIMER_IDLE_COMPLETE_INTERVAL);

	m_lpIdleCompleteTimer->Tick += AddressOf<CPopupWindow>(this, &CPopupWindow::ProcessIdleCompleteTimer);
}

CPopupWindow::~CPopupWindow()
{
	if (m_lpCurrent != NULL)
	{
		delete m_lpCurrent;
		m_lpCurrent = NULL;
	}

	delete m_lpPopupTimer;
	delete m_lpMouseOverTimer;
	delete m_lpIdleCompleteTimer;

	m_lpInstance = NULL;

	if (!m_vQueue.empty())
	{
		while (!m_vQueue.empty())
		{
			TPopupVectorIter pos = m_vQueue.begin();

			(*pos)->Show();

			m_vQueue.erase(pos);
		}
	}
}

CPopupWindow * CPopupWindow::Instance(BOOL fCreate)
{
	if (m_lpInstance == NULL && fCreate)
	{
		// Constructor sets the m_lpInstance.

		new CPopupWindow();
	}

	return m_lpInstance;
}

void CPopupWindow::Show(CPopup * lpPopup)
{
	m_vQueue.push_back(lpPopup);

	if (m_lpCurrent == NULL)
	{
		TPopupVectorIter pos = m_vQueue.begin();

		m_lpCurrent = *pos;

		m_vQueue.erase(pos);

		ASSERT(m_lpCurrent != NULL);

		CalculatePopupRect();

		if (ShouldShowPopup())
		{
			ShowPopup();
		}
		else
		{
			m_nState = PS_PENDING;

			m_lpPopupTimer->SetInterval(200);
			m_lpPopupTimer->SetRunning(TRUE);
		}
	}
}

ATOM CPopupWindow::CreateClass(LPWNDCLASSEX lpWndClass)
{
	lpWndClass->style = CS_HREDRAW | CS_VREDRAW;
	lpWndClass->hCursor = CNotifierApp::Instance()->GetCursorArrow();

	return CWindow::CreateClass(lpWndClass);
}

HWND CPopupWindow::CreateHandle(DWORD dwExStyle, wstring szWindowName, DWORD dwStyle, int x, int y, int cx, int cy, CWindowHandle * lpParentWindow, HMENU hMenu)
{
	return CWindow::CreateHandle(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
		L"",
		WS_POPUP | WS_CLIPSIBLINGS,
		x, y, cx, cy,
		NULL,
		NULL);
}

LRESULT CPopupWindow::DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	return CWindow::WndProc(uMessage, wParam, lParam);
}

void CPopupWindow::CalculatePopupRect()
{
	SIZE sPopupSize = { m_lpCurrent->GetWidth(), m_lpCurrent->GetHeight() };
	SIZE sOffset = { 0, 0 };

	RECT rc;

	GetTaskbarPopupLocation(sPopupSize, sOffset, &rc);

	m_lpCurrent->SetLocation(&rc);

	TASKBARLOCATION tl;

	GetTaskbarLocation(&tl);

	m_uTaskbarEdge = tl.uEdge;
}

LRESULT CPopupWindow::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_MOUSEMOVE:
		OnMouseMove();
		break;

	case WM_MOUSELEAVE:
		OnMouseLeave();
		break;

	case WM_RBUTTONUP:
		CancelAll();
		return 0;
	}

	if (m_lpCurrent == NULL)
	{
		return CWindow::WndProc(uMessage, wParam, lParam);
	}
	else
	{
		return m_lpCurrent->WndProc(uMessage, wParam, lParam);
	}
}

void CPopupWindow::ProcessIdleCompleteTimer()
{
	m_lpIdleCompleteTimer->SetRunning(FALSE);

	ShowPopup();
}

void CPopupWindow::ProcessPopupTimer()
{
	switch (m_nState)
	{
	case PS_PENDING: AnimatePending(); break;
	case PS_OPENING: AnimateOpening(); break;
	case PS_WAITING: AnimateWaiting(); break;
	case PS_CLOSING: AnimateClosing(); break;
	}
}

void CPopupWindow::ProcessMouseOverTimer()
{
	POINT pt;
	RECT rc;

	GetCursorPos(&pt);
	GetWindowRect(GetHandle(), &rc);

	if (!PtInRect(&rc, pt))
	{
		m_lpMouseOverTimer->SetRunning(FALSE);

		SendMessage(WM_MOUSELEAVE);
	}
}

LRESULT CPopupWindow::OnMouseMove()
{
	if (!m_fMouseOver)
	{
		m_lpMouseOverTimer->SetRunning(TRUE);

		m_fMouseOver = TRUE;

		if (m_nState == PS_WAITING)
		{
			m_lpPopupTimer->SetRunning(FALSE);
		}
	}

	return 0;
}

void CPopupWindow::AnimatePending()
{
	if (ShouldShowPopup())
	{
#ifdef _DEBUG
		ShowPopup();
#else
		m_lpPopupTimer->SetRunning(FALSE);
		m_lpIdleCompleteTimer->SetRunning(TRUE);
#endif
	}
}

void CPopupWindow::AnimateOpening()
{
	if (m_nAnimationStep == ANIMATION_STEPS)
	{
		OpeningComplete();
	}
	else
	{
		UpdateFromAnimationStep();

		m_nAnimationStep++;
	}
}

void CPopupWindow::OpeningComplete()
{
	m_nAnimationStep = ANIMATION_STEPS;
	m_nState = PS_WAITING;

	UpdateFromAnimationStep();

	if (!m_fMouseOver)
	{
		m_lpPopupTimer->SetInterval(m_lpCurrent->GetDuration());
		m_lpPopupTimer->SetRunning(TRUE);
	}
	else
	{
		m_lpPopupTimer->SetRunning(FALSE);
	}
}

void CPopupWindow::AnimateWaiting()
{
	m_lpPopupTimer->SetRunning(FALSE);

	if (!m_vQueue.empty())
	{
		m_nState = PS_WAITING;

		if (m_lpCurrent != NULL)
		{
			delete m_lpCurrent;
		}

		TPopupVectorIter pos = m_vQueue.begin();

		m_lpCurrent = *pos;

		m_vQueue.erase(pos);

		ASSERT(m_lpCurrent != NULL);

		CalculatePopupRect();
		UpdateFromAnimationStep();

		m_lpPopupTimer->SetInterval(m_lpCurrent->GetDuration());
		m_lpPopupTimer->SetRunning(TRUE);

		InvalidateRect(GetHandle(), NULL, TRUE);
	}
	else if (DegradeVisualPerformance())
	{
		CompleteClosing();
	}
	else
	{
		m_nState = PS_CLOSING;
		m_nAnimationStep = ANIMATION_STEPS;

		m_lpPopupTimer->SetInterval(m_lpCurrent->GetAnimationDuration() / ANIMATION_STEPS);
		m_lpPopupTimer->SetRunning(TRUE);

		UpdateFromAnimationStep();

		m_nAnimationStep--;
	}
}

void CPopupWindow::CompleteClosing()
{
	m_nAnimationStep = 0;

	DestroyWindow(GetHandle());
}

void CPopupWindow::AnimateClosing()
{
	if (m_nAnimationStep == 0)
	{
		CompleteClosing();
	}
	else
	{
		UpdateFromAnimationStep();

		m_nAnimationStep--;
	}
}

LRESULT CPopupWindow::OnMouseLeave()
{
	if (m_fMouseOver)
	{
		m_fMouseOver = FALSE;

		if (m_nState == PS_WAITING)
		{
			m_lpPopupTimer->SetInterval(2000);
			m_lpPopupTimer->SetRunning(TRUE);
		}
	}

	return 0;
}

void CPopupWindow::UpdateFromAnimationStep()
{
	BYTE nOpacity;
	INT nOffset;

	if (m_nAnimationStep == ANIMATION_STEPS)
	{
		nOpacity = 255;
		nOffset = 0;
	}
	else
	{
		INT nAnimationHeight = m_lpCurrent->GetHeight() / 2;

		nOpacity = (BYTE)((255 * m_nAnimationStep) / ANIMATION_STEPS);
		nOffset = nAnimationHeight - ((nAnimationHeight * m_nAnimationStep) / ANIMATION_STEPS);
	}

	RECT rc;

	m_lpCurrent->GetLocation(&rc);

	switch (m_uTaskbarEdge)
	{
	case TLE_LEFT:
		rc.left -= nOffset;
		rc.right -= nOffset;
		break;

	case TLE_RIGHT:
		rc.left += nOffset;
		rc.right += nOffset;
		break;

	case TLE_TOP:
		rc.top -= nOffset;
		rc.bottom -= nOffset;
		break;

	case TLE_BOTTOM:
		rc.top += nOffset;
		rc.bottom += nOffset;
		break;
	}

	SetLayeredWindowAttributes(GetHandle(), 0, nOpacity, LWA_ALPHA);

	SetWindowPos(
		GetHandle(),
		NULL,
		rc.left,
		rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CPopupWindow::ShouldShowPopup()
{
	if (RunningWine())
	{
		return TRUE;
	}

	// Has the last mouse movement or keyboard action been too long ago?

	LASTINPUTINFO lii;

	memset(&lii, 0, sizeof(LASTINPUTINFO));

	lii.cbSize = sizeof(LASTINPUTINFO);

	GetLastInputInfo(&lii);

	if (GetTickCount() - lii.dwTime > IDLE_TIME)
	{
		return FALSE;
	}

	// Is the foreground application maximized?

	RECT rcForeground;

	GetClientRect(GetForegroundWindow(), &rcForeground);

	POINT pt = { rcForeground.left, rcForeground.top };

	HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);

	MONITORINFOEX miex;

	memset(&miex, 0, sizeof(MONITORINFOEX));

	miex.cbSize = sizeof(MONITORINFOEX);

	GetMonitorInfo(hMonitor, (LPMONITORINFO)&miex);

	// If not maximized, we can show a popup.

	return !(
		miex.rcMonitor.left == rcForeground.left &&
		miex.rcMonitor.top == rcForeground.top &&
		miex.rcMonitor.right == rcForeground.right &&
		miex.rcMonitor.bottom == rcForeground.bottom
	);
}

void CPopupWindow::ShowPopup()
{
	SendMessage(WM_POPUP_OPENING);

	if (DegradeVisualPerformance())
	{
		OpeningComplete();
	}
	else
	{
		m_nState = PS_OPENING;
		m_nAnimationStep = 0;

		m_lpPopupTimer->SetInterval(m_lpCurrent->GetAnimationDuration() / ANIMATION_STEPS);
		m_lpPopupTimer->SetRunning(TRUE);

		SetLayeredWindowAttributes(GetHandle(), 0, 0, LWA_ALPHA);
	}

	ShowWindow(GetHandle(), SW_SHOWNOACTIVATE);
}

void CPopupWindow::CancelAll()
{
	CPopup * lpPopup;

	while (!m_vQueue.empty())
	{
		TPopupVectorIter pos = m_vQueue.begin();

		lpPopup = *pos;

		m_vQueue.erase(pos);

		delete lpPopup;
	}

	DestroyWindow(GetHandle());
}

void CPopupWindow::ShowNext()
{
	if (m_vQueue.empty())
	{
		CancelAll();
	}
	else
	{
		m_nState = PS_WAITING;

		AnimateWaiting();
	}
}

TPopupVector CPopupWindow::GetPopups() const
{
	TPopupVector vResult;

	if (m_lpCurrent != NULL)
	{
		vResult.push_back(m_lpCurrent);
	}

	for (TPopupVectorConstIter iter = m_vQueue.begin(); iter != m_vQueue.end(); iter++)
	{
		vResult.push_back(*iter);
	}

	return vResult;
}

void CPopupWindow::CancelPopup(CPopup * lpPopup)
{
	if (lpPopup == m_lpCurrent)
	{
		ShowNext();
	}
	else
	{
		TPopupVectorIter pos = find(m_vQueue.begin(), m_vQueue.end(), lpPopup);

		if (pos != m_vQueue.end())
		{
			m_vQueue.erase(pos);

			delete lpPopup;
		}
	}
}

void CPopupWindow::ExtendPopupDuration(INT nDuration)
{
	if (!m_fMouseOver && m_nState == PS_WAITING)
	{
		m_lpPopupTimer->SetInterval(nDuration);
		m_lpPopupTimer->SetRunning(TRUE);
	}
}
