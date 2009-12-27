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

#ifndef _INC_INCLUDE
#define _INC_INCLUDE

#pragma once

// Turn this on to test animations
#define FORCE_ANIMATIONS	1

// Turn this on to have CCurl log to curl-log.txt
#define CURL_VERBOSE_LOG	1

// Turn this on to route automatic update requests to a local server
//#define TEST_AUTOMATIC_UPDATE	1

#define FILECOPY_BUFFER_SIZE	4096
#define MAX_LOG_DUMP		(128 * 1024)

#define USERAGENT 		L"Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)"

#define CHM_FILENAME		L"wave-notify.chm"

#define GLOBAL_MUTEX		L"Local\\{A6A3BA8F-CDFF-48f6-B894-B869E59F2EA0}"

#if defined(_DEBUG) && defined(TEST_AUTOMATIC_UPDATE)
#define VERSION_LINK		L"http://ethan/wave-notify/htdocs/check_version.php"
#else
#define VERSION_LINK		L"http://wave-notify.sourceforge.net/check_version.php"
#endif

#ifdef _DEBUG
#define CURL_TIMEOUT		5 // 5 seconds
#else
#define CURL_TIMEOUT		30 // 30 seconds
#endif

#define CURL_WAIT_TIMEOUT	1000	// 1 second

// User messages
#define WM_NOTIFTICON		(WM_USER + 1)
#define WM_LINK_CLICKED		(WM_USER + 2)
#define WM_LOGIN_STATUS		(WM_USER + 3)
#define WM_LISTENER_STATUS	(WM_USER + 4)
#define WM_POPUP_OPENING	(WM_USER + 5)

#define ID_NOTIFYICON		1

typedef enum
{
	LIS_GOT_KEY,
	LIS_GOT_COOKIE,
	LIS_DONE,
	LIS_FAILED
} LOGIN_STATUS;

typedef enum
{
	LTS_RECONNECTING,
	LTS_CONNECTED,
	LTS_RECEIVED
} LISTENER_THREAD_STATUS;

typedef enum
{
	DT_ABOUT,
	DT_OPTIONS,
	DT_LOGIN
} DIALOG_TYPE;

#include "resource.h"

// Framework includes

#include "types.h"
#include "thread.h"
#include "utf8converter.h"
#include "gdi.h"
#include "lock.h"
#include "registry.h"
#include "log.h"
#include "support.h"
#include "event.h"
#include "curl.h"
#include "windowhandle.h"
#include "window.h"
#include "dialog.h"
#include "notifyicon.h"
#include "popup.h"
#include "flyout.h"
#include "propertysheet.h"
#include "app.h"
#include "datetime.h"
#include "unzip.h"

// Application includes

#include "settings.h"
#include "browser.h"
#include "wave.h"
#include "waverequest.h"
#include "waveresponse.h"
#include "unreadwave.h"
#include "notifierapp.h"
#include "version.h"
#include "logindialog.h"
#include "popups.h"
#include "flyouts.h"
#include "aboutdialog.h"
#include "optionssheet.h"
#include "migration.h"

#endif // _INC_INCLUDE
