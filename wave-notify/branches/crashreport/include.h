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

// Turn this on to test animations. These are automatically disabled with remote
// desktop and Wine when this is not enabled.
#define FORCE_ANIMATIONS	1

// Turn this on to have CCurl log to curl-log.txt.
#define CURL_VERBOSE_LOG	1

// Turn this on to route automatic update requests to a local server.
#define TEST_AUTOMATIC_UPDATE	1

// Turn this on to break every time a LOG(n) is called.
#define BREAK_ON_LOG		1

// Turn this on to test the mechanism to send the log.txt to the server.
#define TEST_LOG_DUMP		1

// Turn this on to send crash reports to the local server.
//#define TEST_CRASH_REPORT	1

#define FILECOPY_BUFFER_SIZE	4096
#define MAX_LOG_DUMP		(128 * 1024)

#define USERAGENT 		L"Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)"

#if _DEBUG
#define GLOBAL_MUTEX		L"Local\\{67992639-4CA3-4f3c-B0D0-5DE370D71C80}"
#else
#define GLOBAL_MUTEX		L"Local\\{A6A3BA8F-CDFF-48f6-B894-B869E59F2EA0}"
#endif

#define HOMEPAGE_LINK		L"http://wave-notify.sourceforge.net/index.php?from=client"
#define CHANGELOG_LINK		L"http://wave-notify.sourceforge.net/changelog.php?from=client"
#define TRANSLATORS_LINK	L"http://wave-notify.sourceforge.net/translators.php?from=client"
#define MANUAL_LINK		L"http://wave-notify.sourceforge.net/manual.php"

#if defined(_DEBUG) && defined(TEST_AUTOMATIC_UPDATE)
#define VERSION_LINK		L"http://ethan/wave-notify/htdocs/check_version.php"
#else
#define VERSION_LINK		L"http://wave-notify.sourceforge.net/check_version.php"
#endif

#define CRASH_REPORTER_PATH	L"crashreporter.exe"
#define CRASH_DUMP_PATH		L"Crash Dumps"

#ifdef TEST_CRASH_REPORT
#define CRASH_SERVER_URL	L"http://ethan/wave-notify/htdocs/crashy/submit.php"
#else
#define CRASH_SERVER_URL	L"http://wave-notify.sourceforge.net/crashy/submit.php"
#endif

// All web request timeouts are registered here (in seconds).

#define WEB_TIMEOUT_LONG	(5 * 60)
#define WEB_TIMEOUT_SHORT	(1 * 60)
#define WEB_TIMEOUT_CHANNEL	(10 * 60)

// The interval in which the curl multi perform method is called (in milliseconds).

#define CURL_WAIT_TIMEOUT	1000

// User messages.

#define WM_NOTIFTICON			(WM_USER + 1)
#define WM_LINK_CLICKED			(WM_USER + 2)
#define WM_WAVE_CONNECTION_STATE	(WM_USER + 3)
#define WM_POPUP_OPENING		(WM_USER + 4)
#define WM_CURL_RESPONSE		(WM_USER + 5)
#define WM_VERSION_STATE		(WM_USER + 6)

#define ID_NOTIFYICON		1

typedef enum
{
	DT_ABOUT,
	DT_OPTIONS,
	DT_LOGIN,
	DT_MAX
} DIALOG_TYPE;

typedef enum
{
	CR_SUSPEND,
	CR_LOCK,
	CR_MAX
} CONNECT_REASON;

// All timer intervals (in milliseconds).

#define TIMER_QUERY_INTERVAL			(2 * 60 * 1000)
#define TIMER_VERSION_INTERVAL			(60 * 60 * 1000)
#define TIMER_VERSION_INTERVAL_INITIAL		(10 * 60 * 1000)
#define TIMER_WORKING_INTERVAL			900
#define TIMER_RECONNECT_INTERVAL		(5 * 1000)

#define TIMER_IDLE_COMPLETE_INTERVAL		(4 * 1000)

#define TIMER_RECONNECT_INTERVAL_INITIAL	(5 * 1000)
#define TIMER_RECONNECT_INTERVAL_MAX		(60 * 1000)

#define TIMER_REREPORT_TIMEOUT			(3 * 60 * 1000)

#include "log.h"
#include "compat.h"
#include "types.h"
#include "thread.h"
#include "utf8converter.h"
#include "windowhandle.h"
#include "gdi.h"
#include "lock.h"
#include "registry.h"
#include "support.h"
#include "delegate.h"
#include "event.h"
#include "mutex.h"
#include "curl.h"
#include "window.h"
#include "dialog.h"
#include "notifyicon.h"
#include "timer.h"
#include "popup.h"
#include "flyout.h"
#include "propertysheet.h"
#include "app.h"
#include "datetime.h"
#include "unzip.h"

#endif // _INC_INCLUDE
