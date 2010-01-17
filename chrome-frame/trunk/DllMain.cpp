#include "stdafx.h"
#include "include.h"

#if _MSC_VER <= 1200

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

#endif

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD  dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
#if _MSC_VER <= 1200
		_Module.Init(ObjectMap, hInstance);
		DisableThreadLibraryCalls(hInstance);
#endif
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
#if _MSC_VER <= 1200
	        _Module.Term();
#endif
		break;
	}

	return TRUE;
}
