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

#if _DEBUG
#define GLOBAL_MUTEX		L"Local\\{67992639-4CA3-4f3c-B0D0-5DE370D71C80}"
#else
#define GLOBAL_MUTEX		L"Local\\{A6A3BA8F-CDFF-48f6-B894-B869E59F2EA0}"
#endif

#define WAVENOTIFY_WND_CLASS	L"GoogleWaveNotifier"

#include "types.h"
#include "support.h"

#endif // _INC_INCLUDE
