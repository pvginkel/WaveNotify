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

WINDOWS_VERSION GetWindowsVersion()
{
	OSVERSIONINFO ovi;

	memset(&ovi, 0, sizeof(OSVERSIONINFO));

	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&ovi);

	if (ovi.dwMajorVersion < 5)
	{
		return WV_WINDOWS_OLDER;
	}
	else if (ovi.dwMajorVersion == 5)
	{
		switch (ovi.dwMinorVersion)
		{
		case 0:
			return WV_WINDOWS_2000;

		default:
			return WV_WINDOWS_XP;
		}
	}
	else if (ovi.dwMajorVersion == 6)
	{
		switch (ovi.dwMinorVersion)
		{
		case 0:
			return WV_WINDOWS_VISTA;

		case 1:
			return WV_WINDOWS_7;

		default:
			return WV_WINDOWS_NEWER;
		}
	}
	else
	{
		return WV_WINDOWS_NEWER;
	}
}
