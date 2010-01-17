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

#ifdef _DEBUG

#define DEBUGBREAK __asm { int 3 }

#else // !_DEBUG

#define DEBUGBREAK

#endif

#define CHROME_FRAME_PROGID L"ChromeTab.ChromeFrame"

// And the actual includes.

#include "resource.h"
#include "types.h"
#include "chrome-frame.h"
#include "chrome_tab_h.h"
#include "chromeframe.h"
#include "chromeframemodule.h"

#endif // _INC_INCLUDE
