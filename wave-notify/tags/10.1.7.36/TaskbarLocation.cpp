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

static void GetTaskbarLocationForWine(LPTASKBARLOCATION lpLocation);
static void GetTaskbarLocationForWindows(LPTASKBARLOCATION lpLocation);

void GetTaskbarLocation(LPTASKBARLOCATION lpLocation)
{
	POINT pBar;

	memset(lpLocation, 0, sizeof(TASKBARLOCATION));

	if (RunningWine())
	{
		GetTaskbarLocationForWine(lpLocation);
	}
	else
	{
		GetTaskbarLocationForWindows(lpLocation);
	}

	pBar.x = lpLocation->rcTaskbar.left + (lpLocation->rcTaskbar.right - lpLocation->rcTaskbar.left) / 2;
	pBar.y = lpLocation->rcTaskbar.top + (lpLocation->rcTaskbar.bottom - lpLocation->rcTaskbar.top) / 2;

	if (pBar.x > lpLocation->rcWorkArea.right)
	{
		lpLocation->uEdge = TLE_RIGHT;
	}
	else if (pBar.x < lpLocation->rcWorkArea.left)
	{
		lpLocation->uEdge = TLE_LEFT;
	}
	else if (pBar.y > lpLocation->rcWorkArea.bottom)
	{
		lpLocation->uEdge = TLE_BOTTOM;
	}
	else if (pBar.y < lpLocation->rcWorkArea.top)
	{
		lpLocation->uEdge = TLE_TOP;
	}
	else
	{
		lpLocation->uEdge = TLE_BOTTOM;
	}
}

BOOL GetTaskbarPopupLocation(SIZE sSize, SIZE sOffset, LPRECT lpLocation)
{
	TASKBARLOCATION tl;

	GetTaskbarLocation(&tl);

	*lpLocation = tl.rcWorkArea;

	switch (tl.uEdge)
	{
	case TLE_LEFT:
		lpLocation->left += sOffset.cx;
		lpLocation->right = lpLocation->left + sSize.cx;
		lpLocation->bottom -= sOffset.cy;
		lpLocation->top = lpLocation->bottom - sSize.cy;
		break;

	case TLE_RIGHT:
		lpLocation->right -= sOffset.cx;
		lpLocation->left = lpLocation->right - sSize.cx;
		lpLocation->bottom -= sOffset.cy;
		lpLocation->top = lpLocation->bottom - sSize.cy;
		break;

	case TLE_TOP:
		lpLocation->right -= sOffset.cx;
		lpLocation->left = lpLocation->right - sSize.cx;
		lpLocation->top  += sOffset.cy;
		lpLocation->bottom = lpLocation->top + sSize.cy;
		break;

	case TLE_BOTTOM:
		lpLocation->right -= sOffset.cx;
		lpLocation->left = lpLocation->right - sSize.cx;
		lpLocation->bottom -= sOffset.cy;
		lpLocation->top = lpLocation->bottom - sSize.cy;
		break;

	default:
		memset(lpLocation, 0, sizeof(RECT));
		return FALSE;
	}

	return TRUE;
}

void SnapTaskbarPopupLocation(LPRECT lpLocation, POINT pt)
{
	TASKBARLOCATION tl;
	RECT rc;
	SIZE sz;

	GetTaskbarLocation(&tl);

	if (!PtInRect(&tl.rcTaskbar, pt))
	{
		return;
	}

	rc = *lpLocation;
	sz.cx = lpLocation->right - lpLocation->left;
	sz.cy = lpLocation->bottom - lpLocation->top;

	if (tl.uEdge == TLE_TOP || tl.uEdge == TLE_BOTTOM)
	{
		rc.left = pt.x - sz.cx / 2;
		rc.right = rc.left + sz.cx;

		if (pt.x > tl.rcWorkArea.left + (tl.rcWorkArea.right - tl.rcWorkArea.left) / 2)
		{
			if (rc.right > lpLocation->right)
			{
				return;
			}
		}
		else
		{
			if (rc.left < lpLocation->left)
			{
				return;
			}
		}
	}
	else
	{
		rc.top = pt.y - sz.cy / 2;
		rc.bottom = rc.top + sz.cy;

		if (pt.y > tl.rcWorkArea.top + (tl.rcWorkArea.bottom - tl.rcWorkArea.top) / 2)
		{
			if (rc.bottom > lpLocation->bottom)
			{
				return;
			}
		}
		else
		{
			if (rc.top < lpLocation->top)
			{
				return;
			}
		}
	}
	
	*lpLocation = rc;
}

static void GetTaskbarLocationForWine(LPTASKBARLOCATION lpLocation)
{
	HMONITOR hMonitor;
	MONITORINFOEX miex;
	POINT p;

	p.x = 0;
	p.y = 0;

	hMonitor = MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY);

	memset(&miex, 0, sizeof(MONITORINFOEX));

	miex.cbSize = sizeof(MONITORINFOEX);

	GetMonitorInfo(hMonitor, (LPMONITORINFO)&miex);

	if (
		miex.rcMonitor.left == miex.rcWork.left &&
		miex.rcMonitor.top == miex.rcWork.top &&
		miex.rcMonitor.right == miex.rcWork.right &&
		miex.rcMonitor.bottom == miex.rcWork.bottom
	) {
		lpLocation->rcTaskbar = miex.rcMonitor;
		lpLocation->rcWorkArea = miex.rcMonitor;

		lpLocation->rcTaskbar.bottom = lpLocation->rcTaskbar.top + 25;
		lpLocation->rcWorkArea.top = lpLocation->rcTaskbar.bottom;
	}
	else
	{
		lpLocation->rcWorkArea = miex.rcWork;

		// Detect where the taskbar is located

		lpLocation->rcTaskbar = miex.rcMonitor;

		if (miex.rcWork.top > miex.rcMonitor.top)
		{
			lpLocation->rcTaskbar.bottom = miex.rcWork.top;
		}
		else if (miex.rcWork.bottom < miex.rcMonitor.bottom)
		{
			lpLocation->rcTaskbar.top = miex.rcWork.bottom;
		}
		else if (miex.rcWork.left > miex.rcMonitor.left)
		{
			lpLocation->rcTaskbar.right = miex.rcWork.left;
		}
		else // if (miex.rcWork.right < miex.rcMonitor.right)
		{
			lpLocation->rcTaskbar.left = miex.rcWork.right;
		}
	}
}

static void GetTaskbarLocationForWindows(LPTASKBARLOCATION lpLocation)
{
	APPBARDATA abd;
	HMONITOR hMonitor;
	MONITORINFOEX miex;

	memset(&abd, 0, sizeof(APPBARDATA));

	abd.cbSize = sizeof(APPBARDATA);

	SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
	
	lpLocation->rcTaskbar = abd.rc;

	POINT p = {
		abd.rc.left + (abd.rc.right - abd.rc.left) / 2,
		abd.rc.top + (abd.rc.bottom - abd.rc.top) / 2
	};

	hMonitor = MonitorFromPoint(p, MONITOR_DEFAULTTOPRIMARY);

	memset(&miex, 0, sizeof(MONITORINFOEX));

	miex.cbSize = sizeof(MONITORINFOEX);

	GetMonitorInfo(hMonitor, (LPMONITORINFO)&miex);

	// Windows 7 reports the work area as including the taskbar, but we need
	// it exclusing the taskbar. Correct.

	if (PtInRect(&miex.rcWork, p))
	{
		POINT pMon = {
			miex.rcWork.left + (miex.rcWork.right - miex.rcWork.left) / 2,
			miex.rcWork.top + (miex.rcWork.bottom - miex.rcWork.top) / 2
		};

		// Whether the taskbar is horizontal or vertical.

		if ((abd.rc.right - abd.rc.left) > (abd.rc.bottom - abd.rc.top))
		{
			// Whether the baskbar is at the top or the bottom.

			if (p.y < pMon.y)
			{
				miex.rcWork.top = abd.rc.bottom;
			}
			else
			{
				miex.rcWork.bottom = abd.rc.top;
			}
		}
		else
		{
			// Whether the taskbar is at the left or right.

			if (p.x < pMon.x)
			{
				miex.rcWork.left = abd.rc.right;
			}
			else
			{
				miex.rcWork.right = abd.rc.left;
			}
		}
	}

	lpLocation->rcWorkArea = miex.rcWork;
}
