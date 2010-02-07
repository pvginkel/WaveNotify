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

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <tchar.h>
#include <strsafe.h>
#include <math.h>
#include <crtdbg.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <sstream>
#include <iomanip>

using namespace std;

#endif // _INC_STDAFX
