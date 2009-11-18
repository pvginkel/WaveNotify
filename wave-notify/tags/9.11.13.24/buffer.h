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

#ifndef _INC_BUFFER
#define _INC_BUFFER

#pragma once

typedef struct tagBUFFER {
	LPBYTE lpData;
	DWORD dwSize;
	DWORD dwOffset;
} BUFFER, * PBUFFER;

PBUFFER Buffer_Create();
void Buffer_Destroy(PBUFFER lpBuffer);
void Buffer_Append(PBUFFER lpBuffer, LPBYTE lpData, DWORD dwSize);
DWORD Buffer_Size(PBUFFER lpBuffer);
DWORD Buffer_Offset(PBUFFER lpBuffer);

#endif // _INC_BUFFER
