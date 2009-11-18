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

#include "stdafx.h"
#include "include.h"

PBUFFER Buffer_Create()
{
	PBUFFER lpResult;

	lpResult = ALLOCA(BUFFER);

	return lpResult;
}

void Buffer_Destroy(PBUFFER lpBuffer)
{
	if (lpBuffer->lpData != NULL)
	{
		FREE(lpBuffer->lpData);
	}

	FREE(lpBuffer);
}

void Buffer_Append(PBUFFER lpBuffer, LPBYTE lpData, DWORD dwSize)
{
	if (dwSize > 0)
	{
		if (lpBuffer->lpData == NULL)
		{
			lpBuffer->lpData = (LPBYTE)ALLOC(dwSize);
		}
		else
		{
			lpBuffer->lpData = (LPBYTE)REALLOC(lpBuffer->lpData, lpBuffer->dwSize + dwSize);
		}

		memcpy(lpBuffer->lpData + lpBuffer->dwSize, lpData, dwSize);

		lpBuffer->dwSize += dwSize;
	}
}

DWORD Buffer_Size(PBUFFER lpBuffer)
{
	return lpBuffer->dwSize;
}

DWORD Buffer_Offset(PBUFFER lpBuffer)
{
	return lpBuffer->dwOffset;
}

