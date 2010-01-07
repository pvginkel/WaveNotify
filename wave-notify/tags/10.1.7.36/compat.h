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

#ifndef _INC_COMPAT
#define _INC_COMPAT

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define WM_WTSSESSION_CHANGE            0x02B1

#define WTS_CONSOLE_CONNECT                0x1
#define WTS_CONSOLE_DISCONNECT             0x2
#define WTS_REMOTE_CONNECT                 0x3
#define WTS_REMOTE_DISCONNECT              0x4
#define WTS_SESSION_LOGON                  0x5
#define WTS_SESSION_LOGOFF                 0x6
#define WTS_SESSION_LOCK                   0x7
#define WTS_SESSION_UNLOCK                 0x8
#define WTS_SESSION_REMOTE_CONTROL         0x9

#define NOTIFY_FOR_ALL_SESSIONS     1
#define NOTIFY_FOR_THIS_SESSION     0

BOOL WINAPI
Compat_WTSRegisterSessionNotification(
    HWND hWnd,
    DWORD dwFlags
    );

BOOL WINAPI
Compat_WTSUnRegisterSessionNotification(
    HWND hWnd
    );


#ifdef __cplusplus
};
#endif

#endif // _INC_COMPAT
