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

#ifndef _INC_STDAFX
#define _INC_STDAFX

#pragma once

#include "targetver.h"

// Lean and mean

#define WIN32_LEAN_AND_MEAN

// And strict

#define STRICT

// Kill some irritating warnings

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4995)
#define STRSAFE_NO_DEPRECATE

#include <winsock2.h>
#include <windows.h>
#include <shlwapi.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <commdlg.h>
#include <shellapi.h>
#include <limits.h>
#include <tchar.h>
#include <mmsystem.h>
#include <commctrl.h>
#include <shlobj.h>
#include <strsafe.h>
#include <prsht.h>
#include <wincrypt.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <sstream>
#include <iomanip>
#include <queue>

using namespace std;

#define ZLIB_WINAPI

#include <curl\curl.h>
#include <zlib\zlib.h>
#include <zlib\zip.h>
#include <zlib\unzip.h>
#include "json\json.h"
#include <gd\gd.h>

#endif // _INC_STDAFX
