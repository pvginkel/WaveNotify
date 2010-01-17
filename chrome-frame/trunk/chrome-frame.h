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

#ifndef _INC_CHROMEFRAME
#define _INC_CHROMEFRAME

#ifdef CHROMEFRAME_EXPORTS
#define DLLFUNC __declspec(dllexport)
#else
#define DLLFUNC __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagCHROMEFRAME { int __unused; } * LPCHROMEFRAME;

typedef void (CALLBACK * URLCALLBACK)(LPCHROMEFRAME, LPCWSTR);
typedef void (CALLBACK * NAVIGATEDCALLBACK)(LPCHROMEFRAME, BOOL);

DLLFUNC LPCHROMEFRAME chromeframe_create(HWND parent, LPRECT rect, LPCWSTR url);
DLLFUNC HWND chromeframe_get_handle(LPCHROMEFRAME handle);
DLLFUNC BOOL chromeframe_set_url_callback(LPCHROMEFRAME handle, URLCALLBACK callback);
DLLFUNC BOOL chromeframe_set_navigated_callback(LPCHROMEFRAME handle, NAVIGATEDCALLBACK callback);
DLLFUNC BOOL chromeframe_add_extension(LPCHROMEFRAME handle, LPCWSTR extension_path);
DLLFUNC HWND chromeframe_create_handle(LPCHROMEFRAME handle);
DLLFUNC BOOL chromeframe_destroy(LPCHROMEFRAME handle);
DLLFUNC BOOL chromeframe_available();

#ifdef __cplusplus
};
#endif

#endif // _INC_CHROMEFRAME
