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

struct tagPOPUP;

typedef void (CALLBACK* POPUPPAINTPROC)(HWND, LPPAINTSTRUCT, struct tagPOPUP *);
typedef void (CALLBACK* POPUPPROC)(struct tagPOPUP *);

typedef struct tagPOPUP
{
	INT nDuration;
	INT nHeight;
	INT nWidth;
	INT_PTR nTag;
	INT nAnimationDuration;
	POPUPPAINTPROC lpPaintProc;
	POPUPPROC lpClickedProc;
	POPUPPROC lpClosedProc;
	RECT rLocation;
	TASKBARLOCATIONEDGE uTaskbarEdge;
} POPUP, * LPPOPUP;

LPPOPUP Popup_Create();
void Popup_Show(LPPOPUP lpPopup);
void Popup_Destroy();
void Popup_CancelAll();

#endif // _INC_POPUP
