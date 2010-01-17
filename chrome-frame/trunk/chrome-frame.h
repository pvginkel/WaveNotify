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
