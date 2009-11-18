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

#include "log.h"
#include "support.h"
#include "list.h"
#include "buffer.h"
#include "curl.h"
#include "json.h"
#include "wave.h"
#include "popup.h"
#include "resource.h"
#include "notifyicon.h"
#include "app.h"
#include "loginform.h"
#include "version.h"
#include "newversionform.h"
#include "legacy.h"
#include "flyout.h"

extern HINSTANCE g_hInstance;

// Default USERAGENT of Firefox version 3.5.3.

#define USERAGENT 	L"Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)"

// Used as a max length of getting values from, and writing values to the
// registry.

#define REGVALUE_SIZE	1024

// Default timeout for all CURL request

#ifdef _DEBUG
#define CURL_TIMEOUT	5 // 5 seconds
#else
#define CURL_TIMEOUT	30 // 30 seconds
#endif

// When testing visual animations, enable the definition below to force them

//#define FORCE_ANIMATIONS

// Use this to read the page from disc. This suppresses the login and
// communication to retrieve the wave website. To get the correct contents
// for this file, go to your waves website, choose "View Source" and save
// the result into the file.

//#define WAVES_DEBUG_INPUT "page-full.html"

#endif // _INC_INCLUDE
