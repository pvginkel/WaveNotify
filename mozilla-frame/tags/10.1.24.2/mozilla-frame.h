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

#ifndef _INC_MOZILLAFRAME
#define _INC_MOZILLAFRAME

#ifdef MOZILLAFRAME_EXPORTS
#define DLLFUNC __declspec(dllexport)
#else
#define DLLFUNC __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagMOZILLAFRAME { int __unused; } * LPMOZILLAFRAME;

typedef void (CALLBACK * MFBEFORENAVIGATE)(LPMOZILLAFRAME, LPCWSTR, LPBOOL);
typedef void (CALLBACK * MFNAVIGATECOMPLETE)(LPMOZILLAFRAME, LPCWSTR);

DLLFUNC LPMOZILLAFRAME mozillaframe_create(HWND parent, LPRECT rect);
DLLFUNC BOOL mozillaframe_destroy(LPMOZILLAFRAME handle);
DLLFUNC BOOL mozillaframe_navigate(LPMOZILLAFRAME handle, LPCWSTR url);
DLLFUNC HWND mozillaframe_get_handle(LPMOZILLAFRAME handle);
DLLFUNC LPVOID mozillaframe_get_param(LPMOZILLAFRAME handle);
DLLFUNC BOOL mozillaframe_set_param(LPMOZILLAFRAME handle, LPVOID param);
DLLFUNC BOOL mozillaframe_set_before_navigate_callback(LPMOZILLAFRAME handle, MFBEFORENAVIGATE callback);
DLLFUNC BOOL mozillaframe_set_navigate_complete_callback(LPMOZILLAFRAME handle, MFNAVIGATECOMPLETE callback);

#ifdef __cplusplus
};
#endif

#endif // _INC_MOZILLAFRAME
